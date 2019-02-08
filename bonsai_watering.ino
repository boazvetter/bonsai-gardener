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
    drawTree();
    pinMode(10, OUTPUT); // Peristaltic pump
    digitalWrite(10, HIGH); 
}

Adafruit_GFX_Button *buttons[] = {&on_btn, &off_btn, NULL}; // Array of buttons. Separate lists for separate buttons?

// Update the button state and redraw
bool update_button(Adafruit_GFX_Button *b, bool down) 
{
    b->press(down && b->contains(pixel_x, pixel_y));
    if (b->justReleased())
        b->drawButton(false);
    if (b->justPressed())
        b->drawButton(true);
    return down;
}

// Process all buttons
bool update_button_list(Adafruit_GFX_Button **pb) 
{
    bool down = Touch_getXY();
    for (int i = 0 ; pb[i] != NULL; i++) {
        update_button(pb[i], down);
    }
    return down;
}

void loop(void)
{
    update_button_list(buttons);  //use helper function
    if (on_btn.justPressed()) {
        digitalWrite(10, HIGH);
        delay(100);
    }
    if (off_btn.justPressed()) {
        digitalWrite(10, LOW);
        delay(100);
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
    tft.setFont(NULL);
}

void initializeTFT(){
    uint16_t ID = tft.readID();
    if (ID == 0xD3) ID = 0x9481;
    tft.begin(ID);
    tft.setRotation(0);
    tft.fillScreen(BLACK);
    showmsg(20, 24, 3, NULL, "Bonsai v0.2");
    //showmsg(8, 48, 2, NULL, "watering system 0.1");
    showmsg(80, 230, 2, NULL, "Humidity: ");
    showmsg(90, 300, 1, &FreeSevenSegNumFont, "67");
    //tft.drawRect(200,150,20,150,WHITE);  // Draw Fuel Rect
    
    //button stuff
    on_btn.initButton(&tft,  40, 250, 40, 30, GREEN, GREEN, BLACK, "ON", 2);
    off_btn.initButton(&tft, 40, 290, 40, 30, GREEN, GREEN, BLACK, "OFF", 2);
    on_btn.drawButton(false);
    off_btn.drawButton(false);   
}

void drawTree(){
  showmsg(0, 60, 1, NULL, "                #(");  
  showmsg(0, 70, 1, NULL, "            /&%&&&%&%(");
  showmsg(0, 80, 1, NULL, "         (%%&&%@%@%#%/(,(.");
  showmsg(0, 90, 1, NULL, "       *#(((%%##/&/#&/##%%%(,");
  showmsg(0, 100, 1, NULL, "     ##%#&%@%&&&%%%%#%@&,,.,");
  showmsg(0, 110, 1, NULL, "         #%&*&%(&%%##%##%(##%#");
  showmsg(0, 120, 1, NULL, "            *&&&&#&%&&%&%#(#/*(");
  showmsg(0, 130, 1, NULL, "    /*#(####(%%##&&&      %#%%#/#*#%*..");
  showmsg(0, 140, 1, NULL, " /%#%%##%%%%&/,/%&&%#       ./,(# %%%%(%");
  showmsg(0, 150, 1, NULL, " .*/%##%%  ./%(   #.%*         #(/ #. //");
  showmsg(0, 160, 1, NULL, "                   #%%\            ");
  showmsg(0, 170, 1, NULL, "                    #%((");
  showmsg(0, 180, 1, NULL, "                     %##.");
  showmsg(0, 190, 1, NULL, "                    *((%,");   
  showmsg(0, 200, 1, NULL, "                *(/(#(%(/*,"); 
  showmsg(0, 210, 1, NULL, "****************************************");      
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
