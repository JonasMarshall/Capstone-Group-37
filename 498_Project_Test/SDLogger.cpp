#include "SDLogger.h"
#include <SPI.h>
#include <SD.h>

char filename[20];

File dataFile;

int chipSelect = 10;


void createNewSdFile() {

  
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized successfully"); 

  //find next file name
  int fileNumber = 1;
  while (fileNumber < 1000) {
      snprintf(filename, sizeof(filename), "RowRec%d.csv", fileNumber);
      if (!SD.exists(filename)) {
          break; 
      }
      fileNumber++;
  }
  Serial.println(filename);
  
  // Create or open a CSV file
  dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
    // Write the header row
    dataFile.print("Time"); // Timestamp in milliseconds
    dataFile.print(",");
    dataFile.print("Distance"); // Distance in meters
    dataFile.print(",");
    dataFile.print("Velocity"); // Speed in meters per second
    dataFile.print(",");
    dataFile.print("Stroke_Rate"); // Speed in meters per second
    dataFile.print(",");
    dataFile.println("Acceleration"); // Acceleration in meters per second squared
    dataFile.close();
    Serial.println("headers written to file");
  } else {
    Serial.println("Error opening file");
  }

}

void dataLogger(float strokeRate[], float acceleration[], float velocity[], float time[], float distance[], int numDataPoints){
  /*
 if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");
  */
  
  // Create or open a CSV file
  dataFile = SD.open(filename, FILE_WRITE);
  if (dataFile) {
      // Write the header row
    for (int i = 0; i<numDataPoints; i++){
      dataFile.print(time[i]); // Timestamp in milliseconds
      dataFile.print(",");
      dataFile.print(distance[i]); // Distance in meters
      dataFile.print(",");
      dataFile.print(velocity[i]); // Speed in meters per second
      dataFile.print(",");
      dataFile.print(strokeRate[i]); // Speed in meters per second
      dataFile.print(",");
      dataFile.println(acceleration[i]); // Acceleration in meters per second squared
    }
    dataFile.close();
    Serial.println("data written to file");
  } else {
    Serial.println("Error opening file");
  }
}
