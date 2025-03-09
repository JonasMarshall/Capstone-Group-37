#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"
#include <Arduino_LSM9DS1.h>
#include "Stroke_Detector.h"

int screen = 0;
int selection = 0;
UBYTE *BlackImage;
int button = 0;
PAINT_TIME currentTime;
static uint32_t start_Millis = 0;
static uint32_t current_Millis = 0;
static uint32_t elapsed_Millis = 0;
unsigned long seconds;
unsigned long minutes;
unsigned long hours;
bool started = false;
int count = 0;

float avgAcceleration;
// const int B1Pin = 2;
// int B1State = 0;
// const int B2Pin = 2;
// int B2State = 0;
// const int B3Pin = 2;
// int B3State = 0;
// const int B4Pin = 2;
// int B4State = 0;
// const int B5Pin = 2;
// int B5State = 0;
// const int B6Pin = 2;
// int B6State = 0;

float data[][2] = {{0.000,1.000},{0.102,0.472},{0.204,-0.369},{0.306,-0.734},{0.408,-0.392},{0.510,0.228},{0.612,0.535},{0.714,0.320},{0.816,-0.135},{0.918,-0.388},{1.020,-0.256},{1.122,0.074},{1.224,0.279},{1.327,0.203},{1.429,-0.035},{1.531,-0.199},{1.633,-0.159},{1.735,0.012},{1.837,0.141},{1.939,0.124},{2.041,0.002},{2.143,-0.099},{2.245,-0.095},{2.347,-0.009},{2.449,0.069},{2.551,0.073},{2.653,0.012},{2.755,-0.048},{2.857,-0.055},{2.959,-0.013},{3.061,0.033},{3.163,0.041},{3.265,0.013},{3.367,-0.022},{3.469,-0.031},{3.571,-0.011},{3.673,0.014},{3.776,0.023},{3.878,0.010},{3.980,-0.009},{4.082,-0.017},{4.184,-0.008},{4.286,0.006},{4.388,0.012},{4.490,0.007},{4.592,-0.004},{4.694,-0.009},{4.796,-0.006},{4.898,0.002},{5.000,0.007}};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Test");
  System_Init();
  if(USE_IIC) {
    Serial.print("Only USE_SPI_4W, Please revise DEV_config.h !!!");
    return;
  }

  Serial.print(F("OLED_Init()...\r\n"));
  OLED_1in5_rgb_Init();
  Driver_Delay_ms(500); 
  OLED_1in5_rgb_Clear();  

  Serial.print("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
  Paint_SetScale(65);
  // pinMode(B1Pin, INPUT);
  home_screen();
  // graphical_data();
  // plotter();
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

}

void get_Acceleration(){
  float x, y, z;
  float lowPassAcceleration = 0;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    avgAcceleration = x+y+z;
  }
  Serial.println(avgAcceleration);
}


void home_screen(){
  Paint_DrawString_EN(20, 0, "Start", &Font16, WHITE, BLUE);
  Paint_DrawString_EN(20, 50, "Prev Data", &Font16, WHITE, BLUE);
  Paint_DrawCircle(10, 6, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawCircle(10, 56, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

void home_screen_1(){
  Paint_DrawCircle(10, 6, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawCircle(10, 56, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawCircle(10, 56, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

void home_screen_2(){
  Paint_DrawCircle(10, 6, 5, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawCircle(10, 6, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawCircle(10, 56, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

void timer(){
  current_Millis = millis(); 
  elapsed_Millis = current_Millis - start_Millis;
  seconds = elapsed_Millis /1000;
  minutes = seconds/60;
  hours = minutes/60;
  currentTime.Sec = seconds %60;
  currentTime.Min = minutes %60;
  currentTime.Hour = hours %24;
}

void numerical_data(){
  timer();
  String velocity = String(data[count][1]);
  String avgA = String(avgAcceleration);
  Paint_DrawString_EN(0, 0, "Avg Acc:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 20, "Avg Vel:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 40, "Str/Min:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 60, "Dist:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 80, "Time:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 0, avgA.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 20, velocity.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 40, "3", &Font12,  BLACK, BLUE);
  Paint_DrawString_EN(90, 60, "4", &Font12, BLACK, BLUE);
  Paint_DrawTime(50, 80,&currentTime,&Font12,BLACK, BLUE);
  count++;
}

void graphical_data(){
  Paint_DrawLine(10, 110, 120, 110, GBLUE, DOT_PIXEL_2X2, LINE_STYLE_SOLID); // x axis
  Paint_DrawLine(10, 10, 10, 110, GBLUE, DOT_PIXEL_2X2, LINE_STYLE_SOLID); // y axis
  Paint_DrawString_EN(50, 115, "Time", &Font12, BLACK, BLUE);
  // Paint_DrawString_EN(0, 10, "A", &Font12, BLACK, BLUE);
  // Paint_DrawString_EN(0, 20, "c", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 30, "A", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 40, "c", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 50, "c", &Font12, BLACK, BLUE);
  // Paint_DrawString_EN(0, 60, "e", &Font12, BLACK, BLUE);
  // Paint_DrawString_EN(0, 70, "r", &Font12, BLACK, BLUE);
  // Paint_DrawString_EN(0, 80, "y", &Font12, BLACK, BLUE);
}

void stop_screen(){
  Paint_DrawString_EN(20, 0, "Continue Run", &Font16, WHITE, BLUE);
  Paint_DrawString_EN(20, 50, "Stop", &Font16, WHITE, BLUE);
  Paint_DrawCircle(10, 6, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawCircle(10, 56, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

void minmax(float points[][2], float &xMin, float &xMax, float &yMin, float &yMax, int numPoints){
  xMax = points[0][0];
  xMin = points[0][0];
  yMin = points[0][1];
  yMax = points[0][1];
  for (int i = 1; i<numPoints; i++){
    if (points[i][0]> xMax){
      xMax = points[i][0];
    }
    if (points[i][0]<xMin){
      xMin = points[i][0];
    }
    if (points[i][1]>yMax){
      yMax = points[i][1];
    }
    if (points[i][1]<yMin){
      yMin = points[i][1];
    }
  }
}
int scaleValue(float value, float inMin, float inMax, float outMin, float outMax) {
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void scalePoints(float points[][2], int numPoints, float xMin, float xMax, float yMin, float yMax, float screenWidth, float screenHeight){
  for (int i = 0; i < numPoints; i++) {
        points[i][0] = scaleValue(points[i][0], xMin, xMax, 20, screenWidth);   // Scale X
        points[i][1] = scaleValue(points[i][1], yMin, yMax, screenHeight, 10); // Scale Y
    }
}

void zeroScale(float points[][2], int numPoints, float xMin, float xMax, float screenWidth){
    for (int i = 0; i < numPoints; i++) {
        points[i][0] = scaleValue(points[i][0], xMin, xMax, 20, screenWidth);   // Scale X
        points[i][1] = 40; // Scale Y
    }
}

void plotter(){
  float xMin;
  float xMax;
  float yMin;
  float yMax;
  float width =120.0;
  float height = 100.0;
  // int numPoints = sizeof(StrokeData)/sizeof(StrokeData[0]);
  int numPoints = dist;
  float plottedPoints[dist][2];

  for (int i = 0; i<dist; i++){
    for(int j = 0; j<2;j++){
        plottedPoints[i][j] = StrokeData[i][j];
    }
  }
  if (abs(plottedPoints[0][1]-plottedPoints[dist-1][1]) > 0.02){
    minmax(plottedPoints,xMin, xMax, yMin, yMax, numPoints);
    scalePoints(plottedPoints, numPoints, xMin, xMax, yMin, yMax, width, height);
  }
  else{
    minmax(plottedPoints,xMin, xMax, yMin, yMax, numPoints);
    zeroScale(plottedPoints, numPoints, xMin, xMax, width);
  }
  for (int i = 0; i < numPoints; i++) {
    Paint_DrawPoint(plottedPoints[i][0], plottedPoints[i][1], RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    Serial.print(plottedPoints[i][0]);
    Serial.print(",");
    Serial.println(plottedPoints[i][1]);
  }
  for (int i = 0; i < numPoints-1; i++) {
    Paint_DrawLine(plottedPoints[i][0], plottedPoints[i][1], plottedPoints[i+1][0], plottedPoints[i+1][1],BLUE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  }
}


void loop() {
  // Getting button input
  Serial.println("Enter button choice"); 
  button = Serial.parseInt();
  // B1State = digitalRead(B1Pin);

  if ((button == 1) ||(button == 2) ||(button == 3) ||(button == 4)){
    if((screen == 4) ||(screen == 5)){
      if (button == 4){
        OLED_1in5_rgb_Clear();
        // stop_screen();
        screen = 3;
      }
      else if(button == 1){
        OLED_1in5_rgb_Clear();
        if (screen <5){
          screen = screen +1;
        }
        else{
          screen = 4;
        }

      }
      else if(button == 2){
        OLED_1in5_rgb_Clear();
        if (screen > 4){
          screen = screen -1;
        }
        else{
          screen = 5;
        }
      }
    }  
    else if ((screen == 0) || (screen == 3)){
      if(button == 1) {
          screen = 1;
          // home_screen_1();
      }
      else if (button == 2){
          screen = 2;
          // home_screen_2();
      }
    }
    else if (screen == 1){
      if (button == 3){
          if (started == true){
            OLED_1in5_rgb_Clear();
            screen = 4;
          }
          else{
            OLED_1in5_rgb_Clear();
            screen = 4;
            start_Millis = millis();
            started = true;
            // numerical_data();
          }

      }
      else if (button == 2){
          screen = 2;
          // home_screen_2();
      }
    }
    if (screen == 2){
      if (button == 3){
          if (started == true){
            OLED_1in5_rgb_Clear();
            // home_screen();
            screen = 0;
            started = false;
          }
          else{
            return;
          }
      }
      else if (button == 1){
          screen = 1;
          // home_screen_1();
      }
    }
  }
  switch (screen){
    case 0:
      home_screen();
      break;
    case 1:
      home_screen_1();
      break;
    case 2:
      home_screen_2();
      break;
    case 3:
      stop_screen();
      break;
    case 4:
      get_Acceleration();
      numerical_data();
      break;
    case 5:
      strokeLoop();
      OLED_1in5_rgb_Clear();
      graphical_data();  
      plotter();
      break;
  }
}
