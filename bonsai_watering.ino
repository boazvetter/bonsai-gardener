/* Bonsai Watering System 
 * Measures soil humidity and waters plants automatically. Uses a touchscreen tft interface.
 * Author: Boaz Vetter 
 * Year: 2019
 */
#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000
#include <FreeDefaultFonts.h>

// Setup touch screen
const int XP=8,XM=A2,YP=A3,YM=9; //ID=0x9341
const int TS_LEFT=131,TS_RT=909,TS_TOP=70,TS_BOT=885;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
Adafruit_GFX_Button on_btn, off_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars

#define GREY    0x8410
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

int currentHumidity = 0;

void setup(void)
{
    Serial.begin(9600);    
    initializeTFT();
    pinMode(10, OUTPUT); // Peristaltic pump
    digitalWrite(10, HIGH); 
}

void loop(void)
{
    bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    if (on_btn.justReleased())
        on_btn.drawButton();
    if (off_btn.justReleased())
        off_btn.drawButton();
    if (on_btn.justPressed()) {
        tft.fillRect(40, 80, 160, 60, GREEN);
        digitalWrite(10, HIGH);
    }
    if (off_btn.justPressed()) {
        tft.fillRect(40, 80, 160, 60, RED);
        digitalWrite(10, LOW);
    }    

}

void showmsg(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    //tft.drawFastHLine(0, y, tft.width(), WHITE);
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(GREEN);
    tft.setTextSize(sz);
    tft.print(msg);
}

void initializeTFT(){
    uint16_t ID = tft.readID();
    if (ID == 0xD3) ID = 0x9481;
    tft.begin(ID);
    tft.setRotation(0);
    tft.fillScreen(BLACK);
    showmsg(70, 24, 3, NULL, "Bonsai");
    showmsg(8, 48, 2, NULL, "watering system 0.1");
    showmsg(70, 220, 2, NULL, "Humidity: ");
    showmsg(90, 300, 1, &FreeSevenSegNumFont, "67");
    tft.drawRect(200,150,20,150,WHITE);  // Draw Fuel Rect
    
    //button stuff
    on_btn.initButton(&tft,  60, 200, 100, 40, WHITE, CYAN, BLACK, "ON", 2);
    off_btn.initButton(&tft, 180, 200, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    on_btn.drawButton(true);
    off_btn.drawButton(true);
    tft.fillRect(40, 80, 160, 60, RED);    
}

void setHumidity(int humidity){
  if(humidity < currentHumidity){
    tft.fillRect(200,150,20,150,BLACK);  // Draw Fuel Rect
    tft.drawRect(200,150,20,150,WHITE);  // Draw Fuel Rect    
  }
  int mapped_humid = -(humidity/0.67)+1;
  tft.fillRect(201, 299, 18,  mapped_humid, GREEN); // 285 is max value for y
  currentHumidity = humidity;
}

bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}




///* Bonsai watering system v0.1
// * 
// */
//
//const int minHumidity = 580;   
//const int maxHumidity = 268;  
//int intervals = (minHumidity - maxHumidity)/3;   
//int soilMoistureValue = 0;
//
//void setup() {
//  // put your setup code here, to run once:
//  Serial.begin(9600); // Start serial communication
//  
//
//}
//
//void loop() {
//  int val;
//  soilMoistureValue = analogRead(A0); // Moisture sensor connected to A0
//if(soilMoistureValue > maxHumidity && soilMoistureValue < (maxHumidity + intervals))
//  {
//    Serial.println("Very Wet");
//  }
//else if(soilMoistureValue > (maxHumidity + intervals) && soilMoistureValue < (minHumidity - intervals))
//  {
//    Serial.println("Wet");
//  }
//else if(soilMoistureValue < minHumidity && soilMoistureValue > (minHumidity - intervals))
//  {
//    Serial.println("Dry");
//  }
//delay(100);
//}
//
//
