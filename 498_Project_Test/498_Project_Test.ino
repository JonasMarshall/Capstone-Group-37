#include "OLED_Driver.h"
#include "GUI_paint.h"
#include "DEV_Config.h"
#include "Debug.h"
#include "ImageData.h"

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

int data[][2] = {{5, 12},{6, 78},{9, 10},{10, 20},{13, 7},{21, 90},{30, 40}, {45, 67}, {50, 60}, {67, 14},{70, 80},{80, 9}};

void setup() {
  // put your setup code here, to run once:
  System_Init();
  Serial.begin(9600);
  if(USE_IIC) {
    Serial.print("Only USE_SPI_4W, Please revise DEV_config.h !!!");
    return 0;
  }

  Serial.print(F("OLED_Init()...\r\n"));
  OLED_1in5_rgb_Init();
  Driver_Delay_ms(500); 
  OLED_1in5_rgb_Clear();  

  Serial.print("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
  Paint_SetScale(65);

  home_screen();
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
  Paint_DrawString_EN(0, 0, "Avg Acc:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 20, "Avg Vel:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 40, "Str/Min:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 60, "Dist:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 80, "Time:", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 0, "1", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 20, velocity.c_str(), &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 40, "3", &Font12,  BLACK, BLUE);
  Paint_DrawString_EN(90, 60, "4", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(90, 80, "5", &Font12, BLACK, BLUE);
  Paint_DrawTime(50, 100,&currentTime,&Font12,BLACK, BLUE);
  count++;
}

void update_data(){
  timer();
  Paint_DrawTime(50, 100,&currentTime,&Font12,BLUE, BLACK);
  Paint_NewImage(BlackImage, OLED_1in5_RGB_WIDTH, OLED_1in5_RGB_HEIGHT, 270, BLACK);  
  Paint_SetScale(65);
}

void graphical_data(){
  Paint_DrawLine(10, 110, 120, 110, GBLUE, DOT_PIXEL_2X2, LINE_STYLE_SOLID); // x axis
  Paint_DrawLine(10, 10, 10, 110, GBLUE, DOT_PIXEL_2X2, LINE_STYLE_SOLID); // y axis
  Paint_DrawString_EN(50, 115, "Time", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 10, "V", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 20, "e", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 30, "l", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 40, "o", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 50, "c", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 60, "i", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 70, "t", &Font12, BLACK, BLUE);
  Paint_DrawString_EN(0, 80, "y", &Font12, BLACK, BLUE);
}
void update_graph(){
  Paint_DrawPoint(20, 110, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(30, 100, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(40, 90, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(50, 80, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(60, 70, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(70, 60, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(80, 50, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(90, 40, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(100, 30, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  Paint_DrawPoint(110, 20, RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
}

void stop_screen(){
  Paint_DrawString_EN(20, 0, "Continue Run", &Font16, WHITE, BLUE);
  Paint_DrawString_EN(20, 50, "Stop", &Font16, WHITE, BLUE);
  Paint_DrawCircle(10, 6, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawCircle(10, 56, 5, CYAN, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
}

void minmax(int points[][2], int &xMin, int &xMax, int &yMin, int &yMax, int numPoints){
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
int scaleValue(int value, int inMin, int inMax, int outMin, int outMax) {
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void scalePoints(int points[][2], int numPoints, int xMin, int xMax, int yMin, int yMax, int screenWidth, int screenHeight){
  for (int i = 0; i < numPoints; i++) {
        points[i][0] = scaleValue(points[i][0], xMin, xMax, 20, screenWidth);   // Scale X
        points[i][1] = scaleValue(points[i][1], yMin, yMax, screenHeight, 10); // Scale Y (Inverted for OLED)
    }
}

void plotter(){
  int xMin;
  int xMax;
  int yMin;
  int yMax;
  int width =120;
  int height = 100;
  int numPoints = sizeof(data)/sizeof(data[0]);
  minmax(data,xMin, xMax, yMin, yMax, numPoints);
  scalePoints(data, numPoints, xMin, xMax, yMin, yMax, width, height);
  for (int i = 0; i < numPoints; i++) {
    Paint_DrawPoint(data[i][0], data[i][1], RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
  }
  for (int i = 0; i < numPoints-1; i++) {
    Paint_DrawLine(data[i][0], data[i][1], data[i+1][0], data[i+1][1],BLUE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  }
}


void loop() {
  // Getting button input
  Serial.println("Enter button choice"); 
  button = Serial.parseInt();

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
        if (screen >4){
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
      numerical_data();
      break;
    case 5:
      graphical_data();
      plotter();
      break;
  }


  // if ((screen == 0) || (screen == 1) || (screen == 2) || (screen == 4)){
  //   if(button == 1) {
  //       screen = 1;
  //       home_screen_1();
  //   }
  //   else if (button == 2){
  //       screen = 2;
  //       home_screen_2();
  //   }
  // }
  // if (screen == 1){
  //   if ((button == 3)){
  //       OLED_1in5_rgb_Clear();
  //       if (started == false){
  //         start_Millis = millis();
  //       }
  //       screen = 3;
  //       numerical_data();
  //     }
  // }
  // if(screen == 2){
  //     if (button == 3){
  //       if (started == true){
  //         OLED_1in5_rgb_Clear();
  //         home_screen();
  //         screen = 0;
  //         started = false;
  //       }
  //       else{
  //         return;
  //       }
  //     }
  // }
  // if((screen == 3) || (screen == 5)){
  //   if (button == 4){
  //     OLED_1in5_rgb_Clear();
  //     stop_screen();
  //     screen = 4;
  //   }
  //   else if(button == 1){
  //     OLED_1in5_rgb_Clear();
  //     if (screen == 3){
  //       graphical_data();
  //       screen = 5;
  //     }
  //     else if (screen == 5){
  //       numerical_data();
  //       screen = 3;
  //     }
  //   }
  // }

  // if (screen == 3){
  //   update_data();
  //   started = true;
  // }
  // else if (screen == 5){
  //   update_graph();
  //   started = true;
  // }


}
