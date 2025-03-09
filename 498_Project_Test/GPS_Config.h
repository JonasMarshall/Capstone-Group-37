// GPS Data
extern double lat = 0.0, lon = 0.0;
extern double prevLat = 0.0, prevLon = 0.0;
extern double totalDistance = 0.0;
extern double distance = 0.0;
extern double speed = 0.0; // calculated in m/s

// Settings
extern const double KNOTS_TO_MPS = 0.51444;
extern const double MAX_DISTANCE_THRESHOLD = 5;  // 5 meters
extern const double MAX_SPEED_THRESHOLD = 7;  // max plausible rowing speed in m/s
extern const double MIN_SPEED_THRESHOLD = 0.8;     // max estimated drift speed for row boat in m/s

// Speed smoothing
extern const int SPEED_SAMPLES = 5;
extern double speedHistory[SPEED_SAMPLES];
extern int speedIndex = 0;

bool validateChecksum(String nmeaSentence);
bool parseGPSData(String gpsData);
double haversine(double lat1, double lon1, double lat2, double lon2);
