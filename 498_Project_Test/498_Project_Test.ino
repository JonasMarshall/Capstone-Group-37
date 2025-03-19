#include "OLED_Driver.h"    // Import libraries and files
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"
#include "Stroke_Detector.h"
#include "GPS_Config.h"
#include "SDLogger.h"
#include <Arduino_LSM9DS1.h>

int screen = 0;   // tells you which screen you're on
int button = 0;   // reads keyboard input from serial monitor, will need to get rid of once we implement buttons

UBYTE *BlackImage; // Graphics stuff
PAINT_TIME currentTime; // Time variable for graphics stuff 

static uint32_t start_Millis = 0; //time variables
static uint32_t current_Millis = 0;
static uint32_t elapsed_Millis = 0;


unsigned long seconds;  // holds time
unsigned long minutes;
unsigned long hours;

bool started = false; // checks to see if run started

const int B1Pin = 2;   // for when we switch to buttons
int B1State = 0;
const int B2Pin = 3;
int B2State = 0;
const int B3Pin = 4;
int B3State = 0;
const int B4Pin = 5;
int B4State = 0;
// const int B5Pin = 2;
// int B5State = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);   // Serial stuff
  System_Init();
  OLED_CS_0; // turn screen on
  SD_CS_1; // turn SD off
  SD_System_Init();
  if(USE_IIC) {
    Serial.print("Only USE_SPI_4W, Please revise DEV_config.h !!!");
  }

  // GPS Initialization
  Serial1.begin(9600);

  delay(1000);
  Serial1.println("$PUBX,40,GPRMC,0,1,0,0,0,0*46"); // Enable GPRMC, disable others
  delay(500);

  // Initialize speed history
  for (int i = 0; i < SPEED_SAMPLES; i++) {
    speedHistory[i] = 0;
  }

  Serial.print(F("OLED_Init()...\r\n"));  // Graphing stuff
  OLED_1in5_rgb_Init();
  Driver_Delay_ms(500); 
  OLED_1in5_rgb_Clear();  
  Serial.print("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
  Paint_SetScale(65);

  home_screen();  // start with home screen
  pinMode(B1Pin, INPUT); // buttons
  pinMode(B2Pin, INPUT);
  pinMode(B3Pin, INPUT);
  pinMode(B4Pin, INPUT);
  // pinMode(B5Pin, INPUT);

  if (!IMU.begin()) {     // check accelerometer
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

}
//Drawing functions

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

void timer(){ // timer for display
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
  String velocity_text = String(speed);   // converts the data to strings and displays them
  String avgA_text = String(avgA);
  String distance_text = String(totalDistance);
  String str_text = String(spm);
  // String 500_text = String()split_time
  Paint_DrawString_EN(0, 0, "Avg Acc [m/s^2]:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 20, "Avg Vel [m/s]:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 40, "500m [min]:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 60, "Str/min:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 80, "Dist [m]:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 100, "Time:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 0, avgA_text.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 20, velocity_text.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 40, split_time.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 60, str_text.c_str(), &Font12,  BLACK, BLUE);
  Paint_DrawString_EN(90, 80, distance_text.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawTime(70, 100,&currentTime,&Font12,BLACK, BLUE);
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

void minmax(float points[][2], float &xMin, float &xMax, float &yMin, float &yMax, int numPoints){  // helper function calculates min and max
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
int scaleValue(float value, float inMin, float inMax, float outMin, float outMax) { // helper function, scales the data
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void scalePoints(float points[][2], int numPoints, float xMin, float xMax, float yMin, float yMax, float screenWidth, float screenHeight){
  for (int i = 0; i < numPoints; i++) {
        points[i][0] = scaleValue(points[i][0], xMin, xMax, 20, screenWidth);   // Scale X
        points[i][1] = scaleValue(points[i][1], yMin, yMax, screenHeight, 10); // Scale Y
    }
}

void zeroScale(float points[][2], int numPoints, float xMin, float xMax, float screenWidth){    // when data is zeroes, use this scaling function
    for (int i = 0; i < numPoints; i++) {
        points[i][0] = scaleValue(points[i][0], xMin, xMax, 20, screenWidth);   // Scale X
        points[i][1] = 40; // Scale Y
    }
}

void plotter(){ // plots data
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
  button = 0;
  // Getting button input
  //Serial.println("Enter button choice");  // gets input

  button = Serial.parseInt();
  B1State = digitalRead(B1Pin);
  B2State = digitalRead(B2Pin);
  B3State = digitalRead(B3Pin);
  B4State = digitalRead(B4Pin);
  
  if (B1State == 1) button = 1;
  else if (B2State == 1) button = 2;
  else if (B3State == 1) button = 3;
  else if (B4State == 1) button = 4;

  // Print button states
  // Serial.print("B1State: ");
  // Serial.print(B1State);
  // Serial.print(", B2State: ");
  // Serial.print(B2State);
  // Serial.print(", B3State: ");
  // Serial.print(B3State);
  // Serial.print(", B4State: ");
  // Serial.println(B4State);
  // delay(500);
  
  //on button navigation
  if ((button == 1) ||(button == 2) ||(button == 3) ||(button == 4)){ // check to see if there is button input
    if((screen == 4) ||(screen == 5)){  // if on a data recording screen
      if (button == 4){ // if stop button pressed stop screen
        OLED_1in5_rgb_Clear();
        // stop_screen();
        screen = 3;
      }
      else if(button == 1){ // page up
        OLED_1in5_rgb_Clear();
        if (screen <5){
          screen = screen +1;
        }
        else{
          screen = 4;
        }

      }
      else if(button == 2){ // page down
        OLED_1in5_rgb_Clear();
        if (screen > 4){
          screen = screen -1;
        }
        else{
          screen = 5;
        }
      }
    }  
    else if ((screen == 0) || (screen == 3)){ // if stop screen or intital home screen fill in the first or second dot
      if(button == 1) {
          screen = 1;
          // home_screen_1();
      }
      else if (button == 2){
          screen = 2;
          // home_screen_2();
      }
    }
    else if (screen == 1){  // if screen 1, and press 3, start the run
      if (button == 3){
          if (started == true){ // if the run was started go see the data
            OLED_1in5_rgb_Clear();
            screen = 4;
          }
          else{   // if not started yet, start the run
            OLED_1in5_rgb_Clear();
            screen = 4;
            start_Millis = millis();
            started = true;
            createNewSdFile(); // create a new SD file to save data
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

  // happens every loop
  switch (screen){
    case 0:
      // Serial.println("Loading home screen...");
      home_screen();
      // Serial.println("Home screen completed");
      break;
    case 1:
      // Serial.println("Loading home screen 1...");
      home_screen_1();
      // Serial.println("Home screen 1 completed");
      break;
    case 2:
      // Serial.println("Loading home screen 2...");
      home_screen_2();
      // Serial.println("Home screen 2 completed");
      break;
    case 3:
      // Serial.println("Loading stop screen...");
      stop_screen();
      // Serial.println("Stop screen completed");
      break;
    case 4:
      numerical_data();
      // Serial.println("Loading num loop...");
      numLoop();
      // Serial.println("Num loop completed...");
      // Serial.println("Loading numerical data...");
      numerical_data();
      // Serial.print("Numerical data completed...");
      break;
    case 5:
      graphical_data();
      // Serial.println("Loading stroke loop...");
      strokeLoop();
      // Serial.println("Stroke loop completed...");
      // Ensure proper SPI settings for OLED before attempting to clear
      // Serial.println("Loading OLED...");
      // delay(100);
      //OLED_1in5_rgb_Display(BlackImage);
      OLED_1in5_rgb_Clear();
      OLED_1in5_rgb_Clear();
      // Serial.println("Completed OLED...");
      // Serial.println("Loading Graphical data...");
      graphical_data();
      // Serial.println("Graphical data completed..."); 
      plotter();
      // Serial.println("Graphical data completed...");
      break;
  }
}
