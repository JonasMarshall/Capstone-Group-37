#include <Arduino.h>

// GPS Data
double lat = 0.0, lon = 0.0;
double prevLat = 0.0, prevLon = 0.0;
double totalDistance = 0.0;
double speedKmph = 0.0;

// Settings
const double KNOTS_TO_KMPH = 1.852;
const double MAX_DISTANCE_THRESHOLD = 5;  // 50 meters
const double MAX_SPEED_THRESHOLD = 25.0;  // 25 km/h max plausible rowing speed
const double MIN_SPEED_THRESHOLD = 3;     // max estimated drift speed for row boat

// Speed smoothing
const int SPEED_SAMPLES = 5;
double speedHistory[SPEED_SAMPLES];
int speedIndex = 0;

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
  double rawSpeed = fields[7].toFloat() * KNOTS_TO_KMPH;
  speedHistory[speedIndex] = rawSpeed;
  speedIndex = (speedIndex + 1) % SPEED_SAMPLES;
  
  // Calculate moving average
  speedKmph = 0;
  for (int i = 0; i < SPEED_SAMPLES; i++) {
    speedKmph += speedHistory[i];
  }
  speedKmph /= SPEED_SAMPLES;

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

void loop() {
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();

    if (data.startsWith("$GPRMC") && parseGPSData(data)) {
      // Calculate distance if we have previous valid coordinates
      if (prevLat != 0.0 && prevLon != 0.0) {
        double distance = haversine(prevLat, prevLon, lat, lon);
        
        // Apply thresholds
        if (distance < MAX_DISTANCE_THRESHOLD && 
            speedKmph < MAX_SPEED_THRESHOLD &&
            speedKmph > MIN_SPEED_THRESHOLD) {
          totalDistance += distance;
        }
      }

      // Display results with filtering
      Serial.print("Lat: ");
      Serial.print(lat, 6);
      Serial.print(", Lon: ");
      Serial.print(lon, 6);
      Serial.print(", Filtered Speed: ");
      Serial.print(speedKmph, 2);
      Serial.print(" km/h, Total Distance: ");
      Serial.print(totalDistance, 2); // Distance in meters
      Serial.println(" m");
    }
  }
}