#define DIM_STORAGE 76
#define DIM_COUNTER DIM_STORAGE - 1
#define DIM_FILTER DIM_STORAGE - 4
void strokeLoop();
extern float StrokeData[DIM_FILTER][2];
extern int dist;
extern float avgA;
extern float spm;
extern String split_time;
void numLoop();
void findStrokeMinima(float data[], int &minIndex1, int &minIndex2);