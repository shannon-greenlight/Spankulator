#define OLD_STYLE false
typedef void(*FunctionPointer)();

// Import required libraries
//#include <SPI.h>         // Don't use SPI! Ports are taken
#include <stdlib.h>
#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <Timer5.h>
#include <TerminalVT100.h>
#include <RotaryEncoder.h>
#include <Adafruit_FRAM_I2C.h>
#include <Greenface_EEPROM.h>
#include <EEPROM_Arr16.h>
#include <EEPROM_Int.h>
#include <EEPROM_Bool.h>
#include <EEPROM_String.h>
#include <OLED_Display.h>
#include <WiFiNINA.h>
#include <WIFI_Util.h>

int param_num = 0;
int volatile keypress = 0;
boolean esc_mode = false;
boolean triggered = false;
boolean doing_trigger=false;
int last_encoder_val = 0;
unsigned long last_change_time = 0;
int the_param = 0;
float the_delay = 0;
bool toggle_state = false;
boolean going_up = true;
int lines[] = {LINE_1, LINE_2, LINE_3};
int adj;
int init_adj;
float scale;
int offset;

#include "hardware_defs.h"
#include "general_fxns.h"

#include "SPANK_fxn.h"

#include "face1.h"  // greenface logo art

SPANK_fxn *the_spanker;

// display screen
ArducamSSD1306 display(OLED_RESET);
OLED_Display ui(&display);

int Greenface_EEPROM::eeprom_offset = 16;
EEPROM_Int fxn = EEPROM_Int(0, 1000); // set max to real max when num_fxns has been defined
EEPROM_Bool repeat_on = EEPROM_Bool();  // sizeof fxn val
EEPROM_String wifi_password(64);
EEPROM_String wifi_ssid(64);
EEPROM_Bool wifi_active = EEPROM_Bool();  // sizeof fxn val

#define UP_FXN 0
#define DN_FXN 1
#define STRETCH_FXN 2
#define TOGGGLE_FXN 3
#define MAYTAG_FXN 4
#define LFO_FXN 5
#define USER_FXN 6
#define DVM_FXN 7
#define WIFI_FXN 8

// #include "general_fxns.h"
int check_param(int the_param) {
    Serial.println("Param OK param#: "+ String(the_spanker->param_num));
    switch(the_spanker->param_num) {
      case LONGEST_PULSE:
        the_param = max(the_param,the_spanker->get_param(SHORTEST_PULSE)+1);
        break;
      case SHORTEST_PULSE:
        the_param = min(the_param,the_spanker->get_param(LONGEST_PULSE)-1);
        break;

    }
    return the_param;
}

void put_param_num(uint16_t pnum);
void exe_fxn(void);

#include "up_spanker.h"
#include "dn_spanker.h"
#include "stretch_spanker.h"
#include "toggle_spanker.h"
#include "maytag_spanker.h"

#include "lfo.h"
#include "user_fxn.h"
#include "dvm_fxn.h"
#include "wifi_ui.h"
#include "settings_fxn.h"
#include "wdt.h"

#include "hilevel_fxns.h"

void setup(void)
{
  // Start Serial
  Serial.begin(9600);
  // now set up interrupts
  attachInterrupt(digitalPinToInterrupt(2), intFxnB, RISING);
  attachInterrupt(digitalPinToInterrupt(3), intFxnA, RISING);

  MyTimer5.begin(5);  // 5 times a sec

  // define the interrupt callback function
  MyTimer5.attachInterrupt(heartbeat);

  // start the timer
  MyTimer5.start();

  delay(2000);
  //while (! Serial);
  
  ui.begin(face1);

  up_begin();
  dn_begin();
  stretch_begin();
  toggle_begin();
  maytag_begin();

  lfo_begin();
  user_begin();
  dvm_begin();
  wifi_begin();

  hardware_begin();  
  fxn_begin();

  // check for the WiFi module:
  if (false) { // true forces WiFi
    wifi_ssid.put("Your WiFi");
    wifi_password.put("Your Password");
    wifi_active.set();
  } else {
    //wifi_active.reset();
  }
  if (wifi_active.get() && wifi_status != WL_CONNECTED) {
    wifi_connect();
    delay(2000);
  }

  ui.greet();
  ui.clearDisplay();  
  ui.printText("The Spankulator",0,0,1);
  ui.printText("_______________",0,8,1);
  ui.printText("push any key",0,16,1);
  while(all_buttons_up())   {}


  //fxn.put(0);

  exe_fxn();

}

byte wifi_dly_ctr = 0;

void loop() {
  static boolean trigger_reset=false;
  if (keypress) {
    process_keypress();
  } else {
    housekeep();
  }
  if (triggered) {
    // digitalWrite(triggered_led_pin,HIGH);
    if(!trigger_reset) {
      reset_trigger();
      trigger_reset=true;
    }
    do_trigger();
    if(settings_get_ext_clk()==1) {
      triggered=false;
    } else {
      triggered = repeat_on.get() || doing_trigger || user_doing_trigger;      
    }
    // disable_trigger(triggered);
    //Serial.println("Triggered? "+String(triggered));
  } else {
    if(settings_get_ext_clk()==0) {
      trigger_reset=false;
      digitalWrite(triggered_led_pin,LOW);
    }
  }
  int encoder_val = e.getEncoderValue();
  if (encoder_val != 0 && encoder_val != last_encoder_val) {
    adjust_param(encoder_val);
  } else {
  }
  if(wifi_active.get()) {
    if(true || wifi_dly_ctr++==0) do_server();
  }

}
