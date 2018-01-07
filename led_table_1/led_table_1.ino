/* LedTable 
 * 
  * Written by: Klaas De Craemer, Ing. David Hrbaty, Modified by: Mark Quinn
  *  
  *  
 * Main file with common methods and defines, such as button reading from Bluetooth controller 
 * or setting pixels on the LED area 
 */ 
 //#include <SoftwareSerial.h> 
//LED field size 
#define  FIELD_WIDTH   11
#define  FIELD_HEIGHT  11
const int NUM_LEDS = FIELD_WIDTH * FIELD_HEIGHT;
#define  ORIENTATION_HORIZONTAL //Rotation of table, uncomment to rotate field 90 degrees 


#include <SPI.h>
#include <SD.h>
#define USE_FAST_LED   // FAST_LED as library to control the LED strips 

#define BLUETOOTH_SPEED 115200 
 /* 
  * Some defines used by the FAST_LED library 
  */ 
#define FAST_LED_CHIPSET WS2811
#define FAST_LED_DATA_PIN 6
#define MAX_BRIGHTNESS 255      // Thats full on, watch the power!
#define MIN_BRIGHTNESS 10       // set to a minimum of 8%
const int brightnessInPin = A0;
 
 
 

 
 
#define  NUM_PIXELS    FIELD_WIDTH*FIELD_HEIGHT 
 
 
 /* *** LED color table *** */ 
 #define  BLACK  0x000000
 #define  GREEN  0xFF0000 
 #define  RED    0x00FF00 
 #define  BLUE   0x0000FF 
 #define  YELLOW 0xFFFF00 
 #define  LBLUE  0xFF00FF 
 #define  PURPLE 0x00FFFF 
 #define  WHITE  0XFFFFFF
 #define  AQUA   0xFF00FF
 #define  HOTPINK 0x69FFB4
 unsigned int long colorLib[9] = {RED, GREEN, BLUE, YELLOW, LBLUE, PURPLE, WHITE, AQUA, HOTPINK}; 
 /* *** Game commonly used defines ** */ 
 #define  DIR_UP    1 
 #define  DIR_DOWN  2 
 #define  DIR_LEFT  3 
 #define  DIR_RIGHT 4 
 
 
 /* *** Bluetooth controller button defines and input method *** */ 
 #define  BTN_NONE  0 
 #define  BTN_UP    1 
 #define  BTN_DOWN  2 
 #define  BTN_LEFT  3 
 #define  BTN_RIGHT  4 
 #define  BTN_START  5 
 #define  BTN_EXIT  6 
 
 #define TEXTSPEED 140
 
 uint8_t curControl = BTN_NONE; 
  //Serial1.begin(38400); 

 
 void readInput(){ 
   curControl = BTN_NONE; 
  if (Serial1.available() > 0) { 
     // read the incoming byte: 
    uint8_t incomingByte = Serial1.read(); 
     //Serial.println(incomingByte); 
       switch(incomingByte){ 
         case 51: 
           curControl = BTN_LEFT; 
          break; 
         case 52: 
          curControl = BTN_RIGHT; 
           break; 
        case 49: 
           curControl = BTN_UP; 
           break; 
         case 50: 
          curControl = BTN_DOWN; 
           break; 
        case 53: 
          curControl = BTN_START; 
           break; 
         case 54: 
           curControl = BTN_EXIT; 
          break; 
       } 
     }  
 } 

 
 
 
 /* 
 * FAST_LED implementation  
  */ 
 
 
 
#ifdef USE_FAST_LED
 
#include "FastLED.h" 
#endif
 
 
 CRGB leds[NUM_PIXELS]; 
 
 
 void initPixels(){ 
 
 
  FastLED.addLeds<FAST_LED_CHIPSET, FAST_LED_DATA_PIN, GRB>(leds, NUM_PIXELS); 
 
 
 } 
 
 
 void setPixel(int n, int long color){ 
 

   leds[n] = CRGB(color); 
 } 
 
 
 void setPixelRGB(int n, int long r,int long g, int long b){ 
 
 
   leds[n] = CRGB(r,g,b); 
 } 
 
 
 void setDelay(int duration) { 
   FastLED.delay(duration); 
 } 
 
 
 int getPixel(int n){ 
   return (leds[n].r << 16) + (leds[n].g << 8) + leds[n].b;   
 } 
 
 
 void showPixels(){ 
  int mappedValue = map(analogRead(brightnessInPin), 0, 1023, 0, MAX_BRIGHTNESS);
   FastLED.setBrightness(constrain(mappedValue, MIN_BRIGHTNESS, MAX_BRIGHTNESS));
   FastLED.show(); 
 } 
 
 void setTablePixel(int x, int y, int long color){ 
   #ifdef ORIENTATION_HORIZONTAL 
   setPixel(y%2 ? y*FIELD_WIDTH + x : y*FIELD_WIDTH + ((FIELD_HEIGHT-1)-x),color); 
     #ifdef USE_CONSOLE_OUTPUT 
       setTablePixelConsole(y,x,color); 
    #endif 
   #else 
   setPixel(x%2 ? x*FIELD_WIDTH + ((FIELD_HEIGHT-1)-y) : x*FIELD_WIDTH + y,color); 
     #ifdef USE_CONSOLE_OUTPUT 
      setTablePixelConsole(x,y,color); 
     #endif 
   #endif 
 

}
 
 void setTablePixelRGB(int x, int y, int long r,int long g, int long b){ 
   #ifdef ORIENTATION_HORIZONTAL 
   setPixelRGB(y%2 ? y*FIELD_WIDTH + x : y*FIELD_WIDTH + ((FIELD_HEIGHT-1)-x),r,g,b); 
     #ifdef USE_CONSOLE_OUTPUT 
      setTablePixelConsole(y,x,color); 
     #endif 
  #else 
  setPixelRGB(x%2 ? x*FIELD_WIDTH + ((FIELD_HEIGHT-1)-y) : x*FIELD_WIDTH + y,r,g,b); 
     #ifdef USE_CONSOLE_OUTPUT 
      setTablePixelConsole(x,y,color); 
    #endif 
   #endif 
 } 
 void clearTablePixels(){ 
   for (int n=0; n<FIELD_WIDTH*FIELD_HEIGHT; n++){ 
     setPixel(n,0); 
   } 
 } 
 
 
 /* *** text helper methods *** */ 
 #include "font.h" 
 uint8_t charBuffer[8][8]; 
 
 
 void printText(char* text, unsigned int textLength, int xoffset, int yoffset, int long color){ 
   uint8_t curLetterWidth = 0; 
   int curX = xoffset; 
   clearTablePixels(); 
   
   //Loop over all the letters in the string 
  for (int i=0; i<textLength; i++){ 
    //Determine width of current letter and load its pixels in a buffer 
     curLetterWidth = loadCharInBuffer(text[i]); 
     //Loop until width of letter is reached 
     for (int lx=0; lx<curLetterWidth; lx++){ 
       //Now copy column per column to field (as long as within the field 
      if (curX>=FIELD_WIDTH){//If we are to far to the right, stop loop entirely 
         break; 
       } else if (curX>=0){//Draw pixels as soon as we are "inside" the drawing area 
         for (int ly=0; ly<8; ly++){//Finally copy column 
           setTablePixel(curX,yoffset+ly,charBuffer[lx][ly]*color); 
         } 
       } 
       curX++; 
    } 
   } 
    
   showPixels(); 
 } 

void scrollTextBlocked(char* text, int textLength, int long color) {
  unsigned long prevUpdateTime = 0;
  Serial.println(-textLength);

  for (int x = FIELD_WIDTH; x > -(textLength * 8); x--) {
    printText(text, textLength, x, 2, color);
    //Read buttons
    unsigned long curTime;
    do {
      readInput();
      curTime = millis();
    } while (((curTime - prevUpdateTime) < TEXTSPEED) && (curControl == BTN_NONE));//Once enough time  has passed, proceed

    prevUpdateTime = curTime;
  }
}

 
 //Load char in buffer and return width in pixels 
 uint8_t loadCharInBuffer(char letter){ 
   uint8_t* tmpCharPix; 
   uint8_t tmpCharWidth; 
   
   int letterIdx = (letter-32)*8; 
    
   int x=0; int y=0; 
   for (int idx=letterIdx; idx<letterIdx+8; idx++){ 
     for (int x=0; x<8; x++){ 
      charBuffer[x][y] = ((font[idx]) & (1<<(7-x)))>0; 
     } 
     y++; 
   } 
   
  tmpCharWidth = 8; 
   return tmpCharWidth; 
 } 
 
 
 
 
 /* *********************************** */ 

 
 void fadeOut(){ 
   //Select random fadeout animation 
   int selection = random(3); 
    
   switch(selection){ 
     case 0: 
      //customFadeout();
      //break;
     case 1: 
    { 
       //Fade out by dimming all pixels 
       for (int i=0; i<100; i++){ 
        dimLeds(0.97); 
         showPixels(); 
         delay(10); 
       } 
       break; 
     } 
     case 2: 
     { 
      //Fade out by swiping from left to right with ruler 
       const int ColumnDelay = 10; 
       int curColumn = 0; 
      for (int i=0; i<FIELD_WIDTH*ColumnDelay; i++){ 
         dimLeds(0.97); 
        if (i%ColumnDelay==0){ 
          //Draw vertical line 
          for (int y=0;y<FIELD_HEIGHT;y++){ 
             setTablePixel(curColumn, y, GREEN); 
         } 
          curColumn++; 
         } 
         showPixels(); 
         delay(5); 
       } 
       //Sweep complete, keep dimming leds for short time 
       for (int i=0; i<100; i++){ 
        dimLeds(0.9); 
        showPixels(); 
        delay(5); 
       } 
      break; 
     } 
  } 
 } 
 
 
 void dimLeds(float factor){ 
   //Reduce brightness of all LEDs, typical factor is 0.97 
   for (int n=0; n<(FIELD_WIDTH*FIELD_HEIGHT); n++){ 
     int curColor = getPixel(n); 
     //Derive the tree colors 
     int r = ((curColor & 0xFF0000)>>16); 
     int g = ((curColor & 0x00FF00)>>8); 
     int b = (curColor & 0x0000FF); 
    //Reduce brightness 
    r = r*factor; 
    g = g*factor; 
    b = b*factor; 
    //Pack into single variable again 
     curColor = (r<<16) + (g<<8) + b; 
     //Set led again 
     setPixel(n,curColor); 
   } 
 } 
 
 

 
 void setup(){ 
  Serial.begin(115200); 
   //Wait for serial port to connect 
     Serial1.begin(BLUETOOTH_SPEED);
    
  //Initialise display 
   initPixels(); 
   showPixels(); 
 
 
 
 
   //Init random number generator 
   randomSeed(analogRead(5)/millis());
    /*
   pinMode(53, OUTPUT); // CS/SS pin as output for SD library to work.
      digitalWrite(53, HIGH); // workaround for sdcard failed error...
      if (!SD.begin(8))
      {
        Serial.println("sdcard initialization failed!");
        return;
      }
      Serial.println("sdcard initialization done.");
 */
   mainLoop(); 
 } 
 
 
 void loop(){ 
 } 

