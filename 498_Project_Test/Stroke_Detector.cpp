#include <Arduino_LSM9DS1.h>
#include "Stroke_Detector.h"
#include "SDLogger.h"
#include <SPI.h>
#include <SD.h>

File dataFile;

const int chipSelect = 10; // CS pin for SD card module, will have to change probs

//stroke detector variables
float x, y, z;
float totalAcceleration;

unsigned long second;
int counter = 0;
static uint32_t current_Millis = 0;

float minVal_1;
float minVal_2;
int Mindex =0;
int Maxdex =0;
int temp = 0;
int dist = 0;

int front = 0;
int back = 0;

float avgVelTotal = 0.0;

float StrokeData[72][2];

float filteredpoints[72];
float total_data_points[76];
float new_data[72];


float timeArray[76];
float velArray[76];
float distArray[76];

int BP = 0;

int samplerate = 0;
//call in setup to setup strokeDetector
// void strokeSetup() {
//   if (!IMU.begin()) {
//     Serial.println("Failed to initialize IMU!");
//     while (1);
//   }

//   // Serial.print(IMU.accelerationSampleRate());

//   }

// void setup() {
//   Serial.begin(9600);
//   // delay(10000);  // Optional delay to ensure serial is ready
//   Serial.println("Started");
//   strokeSetup(); //setup stroke detector
// }

//call in loop for stroke detector functionality
void strokeLoop() {
while(true){
  // BP = Serial.parseInt();  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    if (samplerate == 10){
      totalAcceleration = (x+y+z);
    
      current_Millis = millis(); 
      // second = current_Millis /1000;
      // Serial.print(second);
      // Serial.print(",");
      // Serial.print(totalAcceleration);
      // Serial.print("-");
      // Serial.println(counter);
      samplerate = 0;
      timeArray[counter] = current_Millis;
      total_data_points[counter] = totalAcceleration;
      velArray[counter] = 
      counter = counter+1;
      if (counter == 75){
        //dataLogger(float strokeRate, float acceleration, float velocity, float time, float distance);

        //Storing Acceleration data to CSV file and SD card
          // Initialize SD card
        // if (!SD.begin(chipSelect)) {
        //   Serial.println("SD card initialization failed!");
        //   return;
        // }
        // Serial.println("SD card initialized.");
        //   // Create or open a CSV file
        // dataFile = SD.open("data.csv", FILE_WRITE);
        // if (dataFile) {
        //   // Write the header row
        //   for (int i = 0; i<counter;i++){
        //       dataFile.print(timeArray[i]); // Timestamp in milliseconds
        //       dataFile.print(",");
        //       dataFile.print(distArray[i]); // Distance in meters
        //       dataFile.print(",");
        //       dataFile.print(velArray[i]); // Velocity in meters per second
        //       dataFile.print(",");
        //       dataFile.println(total_data_points[i]); // Acceleration in meters per second squared
        //   }
        //   dataFile.close();
        //   Serial.println("data written to data.csv");
        // } else {
        //   Serial.println("Error opening data.csv");
        // }


        // Serial.println("Total Data Points");
        // for(int i = 0; i < 76; i++){
        //   Serial.println(total_data_points[i]);
        // }


        for (int i = 0;i<72;i++){
          filteredpoints[i] =(total_data_points[i] + total_data_points[i+1]+ total_data_points[i+2]+ total_data_points[i+3] + total_data_points[i+4])/5;
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

        dist = Maxdex - Mindex;
        front = dist *0.3;
        // Serial.println("Front");
        // Serial.println(front);
        // Serial.println("Dist");
        // Serial.println(dist);
        for (int k =0;k<dist;k++){
          if(Mindex-front < 0){
            new_data[k] = filteredpoints[k];
          }
          else{
            new_data[k] = filteredpoints[Mindex-front+k];
          }
        }
        Serial.println("New Data Points");
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

// void loop() {
//   strokeLoop();
// }
