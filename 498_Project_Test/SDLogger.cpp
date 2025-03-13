#include "SDLogger.h"
#include <SPI.h>
#include <SD.h>
#include "DEV_Config.h"

char filename[20];

File dataFile;

void SD_System_Init()
{
  //set pin
  pinMode(SD_CS, OUTPUT);
}

void createNewSdFile() {
  SPI.endTransaction();
  delay(100);
  OLED_CS_1; // turn screen off
  SD_CS_0; // turn SD on

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  delay(100);
  if (!SD.begin(SD_CS)) {
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

  SPI.endTransaction();
  SD_CS_1; // turn SD off
  OLED_CS_0; // turn screen on
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));

}

void dataLogger(float strokeRate, float acceleration[], float velocity[], float time[], float distance[], int numDataPoints){
  /*
 if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");
  */
  SPI.endTransaction();
  OLED_CS_1; // turn screen off
  delay(100);
  SD_CS_0; // turn SD on
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
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
      dataFile.print(strokeRate); // Speed in meters per second
      dataFile.print(",");
      dataFile.println(acceleration[i]); // Acceleration in meters per second squared
    }
    dataFile.close();
    Serial.println("data written to file");
  } else {
    Serial.println("Error opening file");
  }
  SPI.endTransaction();
  SD_CS_1; // turn SD off
  delay(100);
  OLED_CS_0; // turn screen on
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
}

bool isSDCardBusy() {
    digitalWrite(SD_CS, LOW); // Select SD card
    SPI.transfer(0xFF);       // Send a dummy byte
    uint8_t response = SPI.transfer(0xFF); // Read response
    digitalWrite(SD_CS, HIGH); // Deselect SD card
    return (response != 0xFF); // If response is not 0xFF, the card is busy
}
