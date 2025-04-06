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
int Mindex = 0;    // index of those values
int Maxdex = 0;
int temp = 0;   // temporary value for swapping mindex and maxdex
int dist = 0;   // index difference between Mindex and Maxdex

int front = 0;  // starting point for each stroke

float StrokeData[DIM_FILTER][2];  // external acceleration stroke curve to be plotted in main

float filteredpoints[DIM_FILTER]; // filtered data

float distance_data_points[DIM_STORAGE]; // stores total distance travelled
float mps_data_points[DIM_STORAGE];    // stores velocity
String split_data_points[DIM_STORAGE]; // stores split speed
float new_data[DIM_FILTER]; //stores the stroke data

float accArray[DIM_STORAGE]; // stores acceleration data
float timeArray[DIM_STORAGE]; // stores time data


int samplerate = 0; // takes measurments every 10 loops

int split_minutes;
float split_seconds;
String split_time = "0:00";

int numCounter = 0; // counter for numloop

float time_start; // for calculation strokes per minute
float time_end;
float strokeTime;
float spm;

void findStrokeMinima(float data[], int &minIndex, int &maxIndex) {
  const int dataSize = DIM_FILTER;

  // Find first minimum
  minIndex = 0;
  float minVal1 = data[0];
  for (int j = 1; j < dataSize; j++) {
    if (data[j] < minVal1) {
      minVal1 = data[j];
      minIndex = j;
    }
  }

  // Find second minimum (at least 5 indices away from the first)
  maxIndex = 0;
  float minVal2 = data[0]; // Arbitrary high value
  for (int j = 0; j < dataSize; j++) {
    if (data[j] < minVal2 && data[j] != minVal1 && (j > minIndex + 5 || j < minIndex - 5)) {
      minVal2 = data[j];
      maxIndex = j;
    }
  }

  // Ensure minIndex is the earlier index
  if (minIndex > maxIndex) {
    int temp = minIndex;
    minIndex = maxIndex;
    maxIndex = temp;
  }
}

void strokeLoop() {
  while(true){
    // GPS calculations - Process any available GPS data
    processGPSData();
    if (IMU.accelerationAvailable()) {
      IMU.readAcceleration(x, y, z);
      if (samplerate == 10){
        totalAcceleration = (x+y+z);
        current_Millis = millis();

        // Calculate split time
        if (speed > 0) {
          split_minutes = (int)(500.0 / speed) / 60;
          split_seconds = (500.0 / speed) - (split_minutes * 60);
          String split_time = String(split_minutes) + ":" + String(split_seconds, 2);
          split_data_points[counter] = split_time;
        } else {
          split_minutes = 0;
          split_seconds = 0;
          split_data_points[counter] = "0:0.00";
        }

        // Store current data points
        timeArray[counter] = current_Millis;
        accArray[counter] = totalAcceleration;
        distance_data_points[counter] = totalDistance;
        mps_data_points[counter] = speed;
        

        counter++;
        samplerate = 0;
        
        if (counter == DIM_COUNTER){
          // Process acceleration data
          for (int i = 0; i < DIM_FILTER; i++){
            filteredpoints[i] = (accArray[i] + accArray[i+1] + accArray[i+2] + accArray[i+3] + accArray[i+4]) / 5;
          }
          
          // Find stroke minima
          int Mindex, Maxdex;
          findStrokeMinima(filteredpoints, Mindex, Maxdex);
          
          // Calculate stroke metrics
          time_start = timeArray[Mindex];
          time_end = timeArray[Maxdex];
          strokeTime = (time_end - time_start) / 1000.0; // Convert to seconds
          spm = 60.0 / strokeTime;

          dist = Maxdex - Mindex;
          
          // Log data
          dataLogger(spm, accArray, mps_data_points, timeArray, distance_data_points, DIM_STORAGE);
          while (isSDCardBusy()) {delay(1);}

          // Prepare stroke data for plotting
          front = dist * 0.3;
          for (int k = 0; k < dist; k++){
            if(Mindex - front < 0){
              new_data[k] = filteredpoints[k];
            } else {
              new_data[k] = filteredpoints[Mindex - front + k];
            }
          }
          
          // for(int i = 0; i < dist; i++){
          //   Serial.print(i);
          //   Serial.print(",");
          //   Serial.println(new_data[i]);
          // }

          // Store stroke data for display
          for(int i = 0; i < dist; i++){
            StrokeData[i][0] = i;
            StrokeData[i][1] = new_data[i];
          }

          // Serial.println("Stroke Data Points");
          // for(int i = 0; i < dist; i++){
          //   Serial.print(StrokeData[i][0]);
          //   Serial.print(",");
          //   Serial.println(StrokeData[i][1]);
          // }

          counter = 0;
          break;
        }
      }
      samplerate++;
    }
  }
}

void numLoop() {
  while(true){
    // GPS calculations
    processGPSData();
    
    if (samplerate == 10) {
      // Calculate average acceleration if needed
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);
        totalAcceleration = (x+y+z);
        avgA = totalAcceleration; // You might want to implement proper averaging here
      }

      // Update split time calculations
      if (speed > 0) {
        split_minutes = (int)(500.0 / speed) / 60;
        split_seconds = (500.0 / speed) - (split_minutes * 60);
      } else {
        split_minutes = 0;
        split_seconds = 0;
      }

      // Serial.println(speed);
      // Store data points for this iteration
      current_Millis = millis();
      timeArray[numCounter] = current_Millis;
      accArray[numCounter] = totalAcceleration;
      distance_data_points[numCounter] = totalDistance;
      mps_data_points[numCounter] = speed;
      
      String split_time = String(split_minutes) + ":" + String(split_seconds, 2);
      split_data_points[numCounter] = split_time;
      
      numCounter++;
      samplerate = 0;
      // Serial.println(numCounter);


      if (numCounter >= DIM_COUNTER) {
        findStrokeMinima(filteredpoints, Mindex, Maxdex);
        numCounter = 0;

        time_start = timeArray[Mindex];
        time_end = timeArray[Maxdex];
        strokeTime = time_end - time_start;
        spm = 60/strokeTime;
        Serial.println("Storing data to SD Card!");
        dataLogger(spm, accArray, mps_data_points, timeArray, distance_data_points, DIM_STORAGE);
        // delay(500);
      }
      break;
    }
    samplerate++;
    
    // delay(100); // Short delay to prevent over-sampling
  }
}