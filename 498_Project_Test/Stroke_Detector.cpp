#include <Arduino_LSM9DS1.h>    // import libraries and files
#include "Stroke_Detector.h"
#include "GPS_Config.h"
#include "SDLogger.h"
#include <SPI.h>
#include <SD.h>
#include "DEV_Config.h"


//stroke detector variables
float x, y, z;
float totalAcceleration;
float avgA;
int counter = 0; // counter for the Strokeloop
static uint32_t current_Millis = 0; // get the current time in milliseconds

float minVal_1;   // minimum values for stroke detection
float minVal_2;
int Mindex =0;    // index of those values
int Maxdex =0;
int temp = 0;   // temporary value for swapping mindex and maxdex
int dist = 0;   // index difference between Mindex and Maxdex

int front = 0;  // starting point for each stroke

float StrokeData[72][2];  // external acceleration stroke curve to be plotted in main

float filteredpoints[72]; // filtered data

float distance_data_points[76]; // stores total distance travelled
float mps_data_points[76];    // stores velocity
String split_data_points[76]; // stores split speed
float new_data[72]; //stores the stroke data

float accArray[76]; // stores acceleration data
float timeArray[76]; // stores time data


int samplerate = 0; // takes measurments every 10 loops

int integer_speed;  // split speed variables
int split_minutes;
float split_seconds;

int numCounter = 0; // counter for numloop


float time_start; // for calculation strokes per minute
float time_end;
float strokeTime;
float spm;

//call in loop for stroke detector functionality
void strokeLoop() {
  while(true){
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
      if (samplerate == 10){
        totalAcceleration = (x+y+z);
        current_Millis = millis();

        // GPS calculations
        if (Serial1.available()) {
          String data = Serial1.readStringUntil('\n');
          data.trim();

          if (data.startsWith("$GPRMC") && parseGPSData(data)) {
            // Calculate distance if we have previous valid coordinates
            if (prevLat != 0.0 && prevLon != 0.0) {
              double distance = haversine(prevLat, prevLon, lat, lon);
              
              // Apply thresholds
              if (distance < MAX_DISTANCE_THRESHOLD && 
                  speed < MAX_SPEED_THRESHOLD &&
                  speed > MIN_SPEED_THRESHOLD) {
                totalDistance += distance;
              }
            }
          }
        }

        integer_speed = round(speed);
        split_minutes = (500/integer_speed) % 60;
        split_seconds = (500/integer_speed) - split_minutes * 60;
        samplerate = 0;
        timeArray[counter] = current_Millis;
        accArray[counter] = totalAcceleration;
        distance_data_points[counter] = distance;
        mps_data_points[counter] = speed;
        String split_time = String(split_minutes) + ":" + String(split_seconds, 2);
        split_data_points[counter] = split_time;

        counter = counter+1;
        if (counter == 75){
          //Store Acceleration data to CSV file and SD card

            for (int i = 0;i<72;i++){
              filteredpoints[i] =(accArray[i] + accArray[i+1]+ accArray[i+2]+ accArray[i+3] + accArray[i+4])/5;
            }
            // Serial.println("Filtered Data Points");
            // for(int i = 0; i < 72; i++){
            //   Serial.println(filteredpoints[i]);
            // }
            minVal_1 = filteredpoints[0];
            for (int j = 0;j<72;j++){
              if (filteredpoints[j] < minVal_1){
                minVal_1 = filteredpoints[j];
                Mindex = j;
              }
            }
            minVal_2 = filteredpoints[0];
            for (int j = 0;j<72;j++){
              if (filteredpoints[j] < minVal_2 && filteredpoints[j]!= minVal_1 && (j > Mindex+ 5 || j < Mindex-5) ){
                minVal_2 = filteredpoints[j];
                Maxdex = j;
              }
            }
            if (Mindex > Maxdex){
              temp = Mindex;
              Mindex = Maxdex;
              Maxdex = temp;
            }
            time_start = timeArray[Mindex];
            time_end = timeArray[Maxdex];
            strokeTime = time_end - time_start;
            spm = 60/strokeTime;

            dist = Maxdex - Mindex;
            OLED_CS_0;
            dataLogger(spm, accArray, mps_data_points, timeArray, distance_data_points, 76);
            OLED_CS_1;
            front = dist *0.3;
            for (int k =0;k<dist;k++){
              if(Mindex-front < 0){
                new_data[k] = filteredpoints[k];
              }
              else{
                new_data[k] = filteredpoints[Mindex-front+k];
              }
            }
            for(int i = 0; i < dist; i++){
              Serial.print(i);
              Serial.print(",");
              Serial.println(new_data[i]);
            }
            for(int i = 0; i < dist; i++){
              StrokeData[i][0] = i;
              StrokeData[i][1] = new_data[i];
            }

            Serial.println("Stroke Data Points");
            for(int i = 0; i < dist; i++){
              Serial.print(StrokeData[i][0]);
              Serial.print(",");
              Serial.println(StrokeData[i][1]);

            }
            counter = 0;
            break;
          }
      }
      samplerate = samplerate + 1;
    }
  }
}

void numLoop() {
  while(true){
    // BP = Serial.parseInt();  
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
      if (samplerate == 10){
        avgA = (x+y+z);
        current_Millis = millis();
            // GPS calculations
        if (Serial1.available()) {
          String data = Serial1.readStringUntil('\n');
          data.trim();

          if (data.startsWith("$GPRMC") && parseGPSData(data)) {
            // Calculate distance if we have previous valid coordinates
            if (prevLat != 0.0 && prevLon != 0.0) {
              double distance = haversine(prevLat, prevLon, lat, lon);
              
              // Apply thresholds
              if (distance < MAX_DISTANCE_THRESHOLD && 
                  speed < MAX_SPEED_THRESHOLD &&
                  speed > MIN_SPEED_THRESHOLD) {
                totalDistance += distance;
              }
            }
          }
        }

        integer_speed = round(speed);
        split_minutes = (500/integer_speed) % 60;
        split_seconds = (500/integer_speed) - split_minutes * 60;

        timeArray[numCounter] = current_Millis;
        accArray[numCounter] = totalAcceleration;
        distance_data_points[numCounter] = distance;
        mps_data_points[numCounter] = speed;
        String split_time = String(split_minutes) + ":" + String(split_seconds, 2);
        split_data_points[numCounter] = split_time;
        numCounter = numCounter + 1;
        samplerate = 0;

        if (numCounter == 75){
          numCounter = 0;
            for (int i = 0;i<72;i++){
            filteredpoints[i] =(accArray[i] + accArray[i+1]+ accArray[i+2]+ accArray[i+3] + accArray[i+4])/5;
          }
          minVal_1 = filteredpoints[0];
          for (int j = 0;j<72;j++){
            if (filteredpoints[j] < minVal_1){
              minVal_1 = filteredpoints[j];
              Mindex = j;
            }
          }
          minVal_2 = filteredpoints[0];
          for (int j = 0;j<72;j++){
            if (filteredpoints[j] < minVal_2 && filteredpoints[j]!= minVal_1 && (j > Mindex+ 5 || j < Mindex-5) ){
              minVal_2 = filteredpoints[j];
              Maxdex = j;
            }
          }
          if (Mindex > Maxdex){
            temp = Mindex;
            Mindex = Maxdex;
            Maxdex = temp;
          }
          time_start = timeArray[Mindex];
          time_end = timeArray[Maxdex];
          strokeTime = time_end - time_start;
          spm = 60/strokeTime;
          OLED_CS_0;
          dataLogger(spm, accArray, mps_data_points, timeArray, distance_data_points, 76);
          OLED_CS_1;
        }
        break;
      }
      samplerate = samplerate + 1;
    } 
  }
}
