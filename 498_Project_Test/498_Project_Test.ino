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

// int data[][2] = {{5, 12},{6, 78},{9, 10},{10, 20},{13, 7},{21, 90},{30, 40}, {45, 67}, {50, 60}, {67, 14},{70, 80},{80, 9}};
// int data[][2] = {{14,4},{18,43},{19,97},{23,64},{25,8},{27,62},{43,62},{45,38},{51,97},{57,100},{61,24},{62,20},{63,28},{65,58},{67,31},{68,88},{73,70},{80,26},{84,47},{88,46},{89,88},{94,81},{97,94},{98,21}};
// float data[][2] = {{0.000,1.000},{0.102,0.472},{0.204,-0.369},{0.306,-0.734},{0.408,-0.392},{0.510,0.228},{0.612,0.535},{0.714,0.320},{0.816,-0.135},{0.918,-0.388},{1.020,-0.256},{1.122,0.074},{1.224,0.279},{1.327,0.203},{1.429,-0.035},{1.531,-0.199},{1.633,-0.159},{1.735,0.012},{1.837,0.141},{1.939,0.124},{2.041,0.002},{2.143,-0.099},{2.245,-0.095},{2.347,-0.009},{2.449,0.069},{2.551,0.073},{2.653,0.012},{2.755,-0.048},{2.857,-0.055},{2.959,-0.013},{3.061,0.033},{3.163,0.041},{3.265,0.013},{3.367,-0.022},{3.469,-0.031},{3.571,-0.011},{3.673,0.014},{3.776,0.023},{3.878,0.010},{3.980,-0.009},{4.082,-0.017},{4.184,-0.008},{4.286,0.006},{4.388,0.012},{4.490,0.007},{4.592,-0.004},{4.694,-0.009},{4.796,-0.006},{4.898,0.002},{5.000,0.007}};
float data[][2] = {{0.00000,1.00000},{0.05051,0.83205},{0.10101,0.48068},{0.15152,0.04780},{0.20202,-0.35496},{0.25253,-0.63390},{0.30303,-0.73401},{0.35354,-0.64846},{0.40404,-0.41562},{0.45455,-0.10547},{0.50505,0.20018},{0.55556,0.42845},{0.60606,0.53204},{0.65657,0.49808},{0.70707,0.34792},{0.75758,0.12876},{0.80808,-0.10023},{0.85859,-0.28319},{0.90909,-0.38064},{0.95960,-0.37745},{1.01010,-0.28404},{1.06061,-0.13148},{1.11111,0.03795},{1.16162,0.18200},{1.21212,0.26857},{1.26263,0.28240},{1.31313,0.22718},{1.36364,0.12278},{1.41414,-0.00103},{1.46465,-0.11271},{1.51515,-0.18662},{1.56566,-0.20867},{1.61616,-0.17857},{1.66667,-0.10852},{1.71717,-0.01919},{1.76768,0.06617},{1.81818,0.12746},{1.86869,0.15229},{1.91919,0.13821},{1.96970,0.09234},{2.02020,0.02873},{2.07071,-0.03566},{2.12121,-0.08531},{2.17172,-0.10973},{2.22222,-0.10549},{2.27273,-0.07635},{2.32323,-0.03171},{2.37374,0.01625},{2.42424,0.05571},{2.47475,0.07801},{2.52525,0.07946},{2.57576,0.06172},{2.62626,0.03088},{2.67677,-0.00441},{2.72727,-0.03525},{2.77778,-0.05467},{2.82828,-0.05911},{2.87879,-0.04895},{2.92929,-0.02803},{2.97980,-0.00239},{3.03030,0.02135},{3.08081,0.03770},{3.13131,0.04343},{3.18182,0.03820},{3.23232,0.02431},{3.28283,0.00592},{3.33333,-0.01212},{3.38384,-0.02552},{3.43434,-0.03151},{3.48485,-0.02937},{3.53535,-0.02039},{3.58586,-0.00738},{3.63636,0.00614},{3.68687,0.01690},{3.73737,0.02257},{3.78788,0.02228},{3.83838,0.01668},{3.88889,0.00761},{3.93939,-0.00241},{3.98990,-0.01089},{4.04040,-0.01594},{4.09091,-0.01668},{4.14141,-0.01336},{4.19192,-0.00715},{4.24242,0.00018},{4.29293,0.00676},{4.34343,0.01109},{4.39394,0.01234},{4.44444,0.01051},{4.49495,0.00634},{4.54545,0.00104},{4.59596,-0.00399},{4.64646,-0.00758},{4.69697,-0.00901},{4.74747,-0.00815},{4.79798,-0.00541},{4.84848,-0.00163},{4.89899,0.00217},{4.94949,0.00509},{5.00000,0.00650}};


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

  // home_screen();
  graphical_data();
  plotter();
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
  Paint_DrawTime(50, 80,&currentTime,&Font12,BLACK, BLUE);
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
        points[i][1] = scaleValue(points[i][1], yMin, yMax, screenHeight, 10); // Scale Y (Inverted for OLED)
    }
}

void plotter(){
  float xMin;
  float xMax;
  float yMin;
  float yMax;
  float width =120.0;
  float height = 100.0;
  int numPoints = sizeof(data)/sizeof(data[0]);
  minmax(data,xMin, xMax, yMin, yMax, numPoints);
  scalePoints(data, numPoints, xMin, xMax, yMin, yMax, width, height);
  for (int i = 0; i < numPoints; i++) {
    Paint_DrawPoint(data[i][0], data[i][1], RED, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    Serial.println(data[i][0]);
    Serial.println(data[i][1]);
  }
  for (int i = 0; i < numPoints-1; i++) {
    Paint_DrawLine(data[i][0], data[i][1], data[i+1][0], data[i+1][1],BLUE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  }
}


void loop() {
  // Getting button input
  // Serial.println("Enter button choice"); 
  // button = Serial.parseInt();

  // if ((button == 1) ||(button == 2) ||(button == 3) ||(button == 4)){
  //   if((screen == 4) ||(screen == 5)){
  //     if (button == 4){
  //       OLED_1in5_rgb_Clear();
  //       // stop_screen();
  //       screen = 3;
  //     }
  //     else if(button == 1){
  //       OLED_1in5_rgb_Clear();
  //       if (screen <5){
  //         screen = screen +1;
  //       }
  //       else{
  //         screen = 4;
  //       }

  //     }
  //     else if(button == 2){
  //       OLED_1in5_rgb_Clear();
  //       if (screen > 4){
  //         screen = screen -1;
  //       }
  //       else{
  //         screen = 5;
  //       }
  //     }
  //   }  
  //   else if ((screen == 0) || (screen == 3)){
  //     if(button == 1) {
  //         screen = 1;
  //         // home_screen_1();
  //     }
  //     else if (button == 2){
  //         screen = 2;
  //         // home_screen_2();
  //     }
  //   }
  //   else if (screen == 1){
  //     if (button == 3){
  //         if (started == true){
  //           OLED_1in5_rgb_Clear();
  //           screen = 4;
  //         }
  //         else{
  //           OLED_1in5_rgb_Clear();
  //           screen = 4;
  //           start_Millis = millis();
  //           started = true;
  //           // numerical_data();
  //         }

  //     }
  //     else if (button == 2){
  //         screen = 2;
  //         // home_screen_2();
  //     }
  //   }
  //   if (screen == 2){
  //     if (button == 3){
  //         if (started == true){
  //           OLED_1in5_rgb_Clear();
  //           // home_screen();
  //           screen = 0;
  //           started = false;
  //         }
  //         else{
  //           return;
  //         }
  //     }
  //     else if (button == 1){
  //         screen = 1;
  //         // home_screen_1();
  //     }
  //   }
  // }
  // switch (screen){
  //   case 0:
  //     home_screen();
  //     break;
  //   case 1:
  //     home_screen_1();
  //     break;
  //   case 2:
  //     home_screen_2();
  //     break;
  //   case 3:
  //     stop_screen();
  //     break;
  //   case 4:
  //     numerical_data();
  //     break;
  //   case 5:
  //     graphical_data();
  //     plotter();
  //     break;
  // }
  // numerical_data();
}
