#define VERSION_NUM "v2.11"
#define OLD_STYLE false
typedef void(*FunctionPointer)();

// Import required libraries
//#include <SPI.h>         // Don't use SPI! Ports are taken
#include <stdlib.h>
#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <Timer5.h>
#include <Adafruit_FRAM_I2C.h>
#include <Greenface_EEPROM.h>
#include <EEPROM_Arr16.h>
#include <EEPROM_Int.h>
#include <EEPROM_Bool.h>
#include <EEPROM_String.h>
#include <WiFiNINA.h>
#include <WIFI_Util.h>
#include <arduino_secrets.h>

//#include "greenface_libs\TerminalVT100\TerminalVT100.h"
#include <TerminalVT100.h>
#include <RotaryEncoder.h>
// #include "greenface_libs\RotaryEncoder\RotaryEncoder.h"
#include "SPANK_ui.h"

int volatile keypress = 0;
boolean esc_mode = false;
boolean triggered = false;
boolean doing_trigger=false;
int last_encoder_val = 0;
unsigned long last_change_time = 0;
float the_delay = 0;
bool toggle_state = false;
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
SPANK_ui ui(&display);

int Greenface_EEPROM::eeprom_offset = 16;
EEPROM_Int fxn = EEPROM_Int(0, 1000); // set max to real max when num_fxns has been defined
EEPROM_Bool repeat_on = EEPROM_Bool();  // sizeof fxn val

#define UP_FXN 0
#define DN_FXN 1
#define STRETCH_FXN 2
#define TOGGLE_FXN 3
#define MAYTAG_FXN 4
#define LFO_FXN 5
#define USER_FXN 6
#define DVM_FXN 7
#define WIFI_FXN 8
#define SETTINGS_FXN 9

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
  begin_all();

  set_encoder();

  // connect if wifi is active
  wifi_attempt_connect(true);

  ui.greet(VERSION_NUM);

  while(keypress==0)   {
    if(wifi_active.get()) {
      do_server();
    }
  }
  // while(all_buttons_up())   {}


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
