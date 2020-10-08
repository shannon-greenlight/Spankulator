// ensure this library description is only included once
#ifndef SPANK_ui_h
#define SPANK_ui_h

#include <ArducamSSD1306.h>    // Modification of Adafruit_SSD1306 for ESP8266 compatibility
#include <Adafruit_GFX.h>   // Needs a little change in original Adafruit library (See README.txt file)

#include "version_num.h"

// OLED Display
/*
HardWare I2C pins
A4   SDA
A5   SCL
*/
#define LINE_0 0
#define LINE_1 20
#define LINE_2 36
#define LINE_3 52

#define FXN_ROW "6"
#define PULSE_LEN_ROW "7"
#define DECAY_ROW "8"
#define RANDOM_ROW "9"
#define SEQUENCE_ROW "8"
#define SEQ_INDEX_ROW "9"
#define USER_VAR_ROW 10
#define TRIGGER_ROW "20"
#define DEBUG_ROW "21"
#define INPUT_ROW "21"
#define MSG_ROW "21"

#define OLED_RESET  16  // Pin 15 -RESET digital signal
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

class SPANK_ui {
  public:
    const int lines[4] = {LINE_1,LINE_2,LINE_3,LINE_0};
    TerminalVT100 t;
    ArducamSSD1306 *display;
    SPANK_ui(ArducamSSD1306 *_display) {
      display = _display;
    }

    void begin(const uint8_t *bitmap) {
      // SSD1306 Init
      (*display).begin();  // Switch OLED
      // Clear the buffer.
      (*display).clearDisplay();
      (*display).stopscroll();
      (*display).setTextColor(WHITE,BLACK);
      //(*display).setRotation(2);
      //(*display).startscrollright(0,15);
      if(true) {
        for(int i=0;i<48;i+=2) {
          clearDisplay();
          //printText("Greenface",0,0,2);
          (*display).drawBitmap(0,64-i,bitmap, 128, 64, WHITE);
          showDisplay();
          //delay(10);
        }
        //clearDisplay();
        fill(BLACK,16);
        //printText("Greenface Labs",0,0,1);
        printText("Greenface Labs",0,0,1);
        (*display).drawBitmap(0,16,bitmap, 128, 64, WHITE);
        showDisplay();
        delay(3000);
        //clearDisplay();
        for(int i=16;i<=64;i+=4) {
          clearDisplay();
          //printText("Greenface",0,0,2);
          (*display).drawBitmap(0,i,bitmap, 128, 64, WHITE);
          showDisplay();
          //delay(2);
        }
      }
      //showDisplay();
      //delay(2000);
      t.begin();
    }

    void clearDisplay() {
      // maybe clr terminal here
        (*display).clearDisplay();
    }
    
    void showDisplay() {
        (*display).display();
    }

    void terminal_printText(String text, int line) {
      String row;
      switch(line) {
        case LINE_1:
          row = PULSE_LEN_ROW;
          break;
        case LINE_2:
          row = DECAY_ROW;
          break;
        case LINE_3:
          row = RANDOM_ROW;
          break;
        case 0:
          row = FXN_ROW;
          text = "Fxn: " + text;
          break;
        default:
          row="-1";
      }
      if(row!="-1") {
        t.setRow(row);
        t.println(text);
      }
    }
    
    void printText(String s, int x=0, int y=0, int fontsize=1) {
      (*display).setCursor(x,y);
      (*display).setTextSize(fontsize);
      (*display).println(s);
      (*display).display();
      terminal_printText(s, y);
    }

    void printScreen(String head, String body, String foot="") {
      printText(head,0,0,2);
      printText(body,0,20,3);
      printText(foot,0,48,2);
    }

    void printLine(String text, int line, int fontsize=2) {
      printText(text,0,line,fontsize);
      //terminalPrintParam(row, label, String(buf));
    }

    void drawHLine(int y, int color=WHITE) {
      (*display).drawFastHLine(0,y,128,color);
    }

    void hiliteLine(uint16_t line_num, int num_lines=3, int offset=9) {
      for(int i=0;i<num_lines;i++) {
        drawHLine(lines[i]+offset,BLACK);
      }
      //Serial.println("Hilighting: "+String(lines[line_num]+offset));
      drawHLine(lines[line_num]+offset,WHITE);
    }

    void hiliteChar(uint16_t line_num, uint8_t char_num ) {
      int num_lines=3;
      int offset=9;
      for(int i=0;i<num_lines;i++) {
        drawHLine(lines[i]+offset,BLACK);
      }
      Serial.println("Hilighting: "+String(lines[line_num]+offset));
      printText("_",char_num,lines[line_num]+offset,1);
      //drawHLine(lines[line_num]+offset,WHITE);
    }

    void fill(int color=WHITE,int offset=0) {
      for(int i=offset;i<64;i++) {
        (*display).drawFastHLine(0,i,128,color);
      }
    }

    void drawPixel(int x, int y, int color=WHITE) {
      (*display).drawPixel(x,y,color);
    }

    void graphData(int* data) {
      for(int i=0;i<128;i++) {
        drawPixel(i,data[i],WHITE);
      }
    }

    const int basex = 116;
    const int basey = 14;

    void printWiFiBar(int x,int h) {
      (*display).drawFastVLine(x,basey-h,h,WHITE);
      (*display).drawFastVLine(x+1,basey-h,h,WHITE);
    }

    void printWiFiBars(long rssi) {
      if(rssi>-80) printWiFiBar(basex,3);
      if(rssi>-70) printWiFiBar(basex+3,6);
      if(rssi>-60) printWiFiBar(basex+6,9);
      if(rssi>-50) printWiFiBar(basex+9,12);
      showDisplay();
    }

    void test() {
      fill();
      showDisplay();
    }

    void terminalPrintParam(String row, String label, String val) {
      t.printVal(row, label, val);
    }

    void terminalSplash() {
      // Use Putty to communicate (VT100 terminal)
      t.clrScreen();
      t.setCursor("1", "1");
      const int stars = 76;
      
      t.printChars(stars, "*");
      t.println("");
      t.printTitle(stars, F("The Spankulator  by  GREENFACE LABS "));
      t.printTitle(stars, VERSION_NUM);
      t.printChars(stars, "*");
      t.println("");
    }

    void newFxn(String fxn) {
      clearDisplay();
      t.clrDown(FXN_ROW);
      terminalSplash();
      // t.setRow(FXN_ROW);
      // t.println("Fxn: " + fxn);
      printLine(fxn,0,2);
    }

    void printParam(String label, int param, int digits, const char* format, uint16_t line_num, int font_size=1,String suffix="") {
      char buf[10] = {0,0,0,0,0,0,0,0,0,0}; // buffer to print up to 9 digits
      //String format = "%"+String(digits)+"d";
      //Serial.println("Format: "+String(format));
      snprintf(buf, digits+1, format, param);
      printLine(label+String(buf)+suffix,lines[line_num],font_size);
      //char* rows[3] = {PULSE_LEN_ROW,DECAY_ROW,RANDOM_ROW};
      String row;
      switch(lines[line_num]) {
        case LINE_1:
          row = PULSE_LEN_ROW;
          break;
        case LINE_2:
          row = DECAY_ROW;
          break;
        case LINE_3:
          row = RANDOM_ROW;
          break;
        default:
          row = "0";
      }
      // terminalPrintParam(row, label, String(buf));
      //t.printVal(row, label, String(buf));
    }

    void underline_char(byte char_num, byte line_num,int font_size=2,int offset=0) {
      String prefix = "";
      for(int i=0;i<char_num;i++) {
        prefix += " ";
      }
      // Serial.println("Prefix: " + prefix);
      fill(BLACK, lines[line_num]+offset);
      printLine(prefix + "-",lines[line_num]+offset,font_size);
      //(*display).println(prefix + "-");
    }

    void splash() {
      clearDisplay();  
      printText(F("The Spankulator"),0,0,1);
      printText(F("_______________"),0,8,1);
      printText(VERSION_NUM " - push any key",0,16,1);

      terminalSplash();

    }

};

#endif
