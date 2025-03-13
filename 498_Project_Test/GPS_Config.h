// GPS Data
extern double lat, lon;
extern double prevLat, prevLon;
extern double totalDistance;
extern double distance;
extern double speed; // calculated in m/s

// Settings
const double KNOTS_TO_MPS = 0.51444;
const double MAX_DISTANCE_THRESHOLD = 5;  // 5 meters
const double MAX_SPEED_THRESHOLD = 6.5;  // max plausible rowing speed in m/s
const double MIN_SPEED_THRESHOLD = 0.7;     // max estimated drift speed for row boat in m/s

// Speed smoothing
const int SPEED_SAMPLES = 5;
extern double speedHistory[SPEED_SAMPLES];
extern int speedIndex;

bool validateChecksum(String nmeaSentence);
bool parseGPSData(String gpsData);
double haversine(double lat1, double lon1, double lat2, double lon2);
void processGPSData();
