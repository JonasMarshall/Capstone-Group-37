#include <Arduino.h>
#include "GPS_Config.h"

// Checksum validation
bool validateChecksum(String nmeaSentence) {
  int asteriskIndex = nmeaSentence.indexOf('*');
  if (asteriskIndex == -1) return false;
  
  String checksumStr = nmeaSentence.substring(asteriskIndex + 1);
  byte actualChecksum = strtol(checksumStr.c_str(), NULL, 16);
  
  byte calculatedChecksum = 0;
  for (int i = 1; i < asteriskIndex; i++) {
    calculatedChecksum ^= nmeaSentence[i];
  }
  
  return (actualChecksum == calculatedChecksum);
}

bool parseGPSData(String gpsData) {
  if (!gpsData.startsWith("$GPRMC") || !validateChecksum(gpsData)) {
    return false;
  }

  String fields[13];
  int fieldCount = 0;
  int start = 0;
  int end = gpsData.indexOf(',');

  while (end != -1 && fieldCount < 12) {
    fields[fieldCount++] = gpsData.substring(start, end);
    start = end + 1;
    end = gpsData.indexOf(',', start);
  }
  fields[fieldCount++] = gpsData.substring(start);

  if (fieldCount < 10 || fields[2] != "A") {
    return false;
  }

  // Parse position data
  String latStr = fields[3];
  String latDir = fields[4];
  String lonStr = fields[5];
  String lonDir = fields[6];
  
  // Parse satellites from GPGGA (need to add this sentence handling)
  // For simplicity, we'll just use basic validation here
  if (latStr.length() < 4 || lonStr.length() < 5) {
    return false;
  }

  // Store previous position
  prevLat = lat;
  prevLon = lon;

  // Convert coordinates
  lat = latStr.substring(0, 2).toDouble() + latStr.substring(2).toDouble() / 60.0;
  if (latDir == "S") lat = -lat;
  
  lon = lonStr.substring(0, 3).toDouble() + lonStr.substring(3).toDouble() / 60.0;
  if (lonDir == "W") lon = -lon;

  // Parse and smooth speed
  double rawSpeed = fields[7].toFloat() * KNOTS_TO_MPS;
  speedHistory[speedIndex] = rawSpeed;
  speedIndex = (speedIndex + 1) % SPEED_SAMPLES;
  
  // Calculate moving average
  speed = 0;
  for (int i = 0; i < SPEED_SAMPLES; i++) {
    speed += speedHistory[i];
  }
  speed /= SPEED_SAMPLES;

  return true;
}

double haversine(double lat1, double lon1, double lat2, double lon2) {
  const double R = 6371.0; // Earth radius in km
  double dLat = (lat2 - lat1) * PI / 180.0;
  double dLon = (lon2 - lon1) * PI / 180.0;
  
  lat1 = lat1 * PI / 180.0;
  lat2 = lat2 * PI / 180.0;

  double a = sin(dLat/2) * sin(dLat/2) + 
             cos(lat1) * cos(lat2) * 
             sin(dLon/2) * sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1-a)); 
  
  return R * c * 1000; // Multiply result to get distance in meters
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  // Initialize speed history
  for (int i = 0; i < SPEED_SAMPLES; i++) {
    speedHistory[i] = 0.0;
  }
  Serial.println("GPS Accuracy Test Initialized...");
}