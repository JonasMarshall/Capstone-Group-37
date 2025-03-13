void dataLogger(float strokeRate, float acceleration[], float velocity[], float time[], float distance[], int numDataPoints);

void createNewSdFile();


#define SD_CS 9
#define SD_CS_0     digitalWrite(SD_CS, LOW)
#define SD_CS_1     digitalWrite(SD_CS, HIGH)

void SD_System_Init();
bool isSDCardBusy();