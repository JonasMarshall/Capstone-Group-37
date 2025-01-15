/*
  Arduino LSM9DS1 - Simple Accelerometer

  This example reads the acceleration values from the LSM9DS1
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  
Arduino Nano 33 BLE Sense

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include <Arduino_LSM9DS1.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  //Serial.println("X\tY\tZ");
  Serial.println("Total acceleration:");
}

void loop() {
  float x, y, z;
  float totalAcceleration;
  float lowPassAcceleration = 0;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    totalAcceleration = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    lowPassAcceleration += (totalAcceleration - lowPassAcceleration) 0.1; //0.1 is time constant
    Serial.println(totalAcceleration);
    //Serial.print(x);
    //Serial.print('\t');
    //Serial.print(y);
    //Serial.print('\t');
    //Serial.println(z);
  }
}
