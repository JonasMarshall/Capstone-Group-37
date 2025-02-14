
#include <Arduino_LSM9DS1.h>

//stroke detector variables
float x, y, z;
float totalAcceleration;
const int samples = 50;
float xReadings[samples];
float yReadings[samples];
float zReadings[samples];
int idx = 0;
float xSum = 0;
float ySum = 0;
float zSum = 0;
float xAvg = 0;
float yAvg = 0;
float zAvg = 0;
float filteredX;
float filteredY;
float filteredZ;
int sampleCount = 0;
float strokeRate = 27;
float aMax = -10000;
float aMin = 10000;
char dominantAxis = 'N';
float directionAcceleration = 0;

//call in setup to setup strokeDetector
void strokeSetup() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.println("Acceleration in g's");
  Serial.println("X\tY\tZ\tfX\tfY\tfZ");
  //Serial.println("Total acceleration:");

  while (!IMU.accelerationAvailable()); //wait for acceleration data to be available
  //get initial 50 readings
  for (int i = 0; i < samples; i++) {
      IMU.readAcceleration(x, y, z);
      //record readings
      xReadings[i] = x;
      yReadings[i] = y;
      zReadings[i] = z;

      //add readings to sum
      xSum += x;
      ySum += y;
      zSum += z;
  }

  xAvg = xSum/(float)samples;
  yAvg = ySum/(float)samples;
  zAvg = zSum/(float)samples; 
}


void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");
  

  strokeSetup(); //setup stroke detector
}

//call in loop for stroke detector functionality
void strokeLoop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    if (sampleCount = 10) {
      xSum = xSum - xReadings[idx] + x;
      ySum = ySum - yReadings[idx] + y;
      zSum = zSum - zReadings[idx] + z;

      xReadings[idx] = x;
      yReadings[idx] = y;
      zReadings[idx] = z;

      xAvg = xSum/(float)samples;
      yAvg = ySum/(float)samples;
      zAvg = zSum/(float)samples; 

      idx = (idx + 1) % samples;
      
      sampleCount = 0;
    }
    sampleCount++;

    filteredX = x - xAvg;
    filteredY = y - yAvg;
    filteredZ = z - zAvg;

    //totalAcceleration = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    //lowPassAcceleration += (totalAcceleration - lowPassAcceleration) * 0.1; //0.1 is time constant
    //Serial.println(totalAcceleration);
  
    
    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.print(z);
    Serial.print('\t');
    Serial.print(filteredX);
    Serial.print('\t');
    Serial.print(filteredY);
    Serial.print('\t');
    Serial.println(filteredZ); 
    

    //stroke rate calculation (just use )

  }
}


void loop() {
  strokeLoop();

}