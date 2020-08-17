#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <TerminalVT100.h>
#include <stdlib.h>

#define LFO_BASE_PERIOD 2
#define LFO_BASE_DELAY 2
#define LFO_PARTS 128

void lfo_fxn();

EEPROM_Int lfo_waveform_num = EEPROM_Int(0, 1); // set max to real max when lfo_num_waveforms has been defined
uint16_t wave_table[128] PROGMEM;
int delay_usecs=39047;
// int lfo_param_num=0;
// uint16_t lfo_period = 9999;
EEPROM_Int lfo_period = EEPROM_Int(10,9990);  // set max to real max when num_fxns has been defined

// void write_dac(uint16_t val) {
//   val = ( val << 2 ) | 0x3000;
//   digitalWrite(dac_select_pin, LOW); //initialize the chip
//   SPI.transfer16(val); // write to device
//   digitalWrite(dac_select_pin, HIGH); // de-initialize the chip
// }

String lfo_params_toJSON() {
    String out = "";
    out += "{ ";
    out += toJSON("label","Period (ms)");
    out += ", ";
    out += toJSON("type","number");
    out += ", ";
    out += toJSON("value",String(lfo_period.get()));
    out += ", ";
    out += toJSON("min",String(lfo_period.min));
    out += ", ";
    out += toJSON("max",String(lfo_period.max));
    out += ", ";
    out += toJSON("selected","true");
    out += " }";
    out += ",";
    out += "{ ";
    out += toJSON("label","Waveform");
    out += ", ";
    out += toJSON("type","text");
    out += ", ";
    out += toJSON("value",lfo_waveform_num.get()==0 ? "Sine" : "Ramp" );
    out += ", ";
    out += toJSON("selected","false");
    out += " }";
    return "["+out+"]";
}

void lfo_print_param() {
    ui.printParam("LFO ",lfo_period.get(),4,"%4d",3,2,"ms");
}

void lfo_set_waveform() {
    int temp;
    switch(lfo_waveform_num.get()) {
    case 0:
        for(int i=0;i<LFO_PARTS;i++) {
            temp = (sin(6.28*i/LFO_PARTS)*511)+512;
            scale_and_offset(&temp);
            wave_table[i] = temp;
        }
        break;
    case 1:
        for(int i=0;i<LFO_PARTS;i++) {
            temp = i*1024/LFO_PARTS;
            scale_and_offset(&temp);
            wave_table[i] = temp;
        }
        break;
    default:
        Serial.println("LFO set waveform unknown table: "+String(lfo_waveform_num.get()));
    }
}

byte lfo_num_params = 2;    // see set_waveform for num cases

void lfo_inc_param_num_by(int val) {
    lfo_waveform_num.inc(val);
    // lfo_param_num += val;
    // if(lfo_param_num>1) lfo_param_num = 0;
    // if(lfo_param_num<0) lfo_param_num = 1;
    lfo_set_waveform();
}

boolean user_adjusting() {
    static int cnt = 0;
    if(true || cnt++>100) {
        cnt=0;
        adj = analogRead(ain1_pin);
        scale = (adj/1023.0);
        offset = (512-adj/2);
        return abs(adj-init_adj)>40;
    } else {
        //Serial.println(cnt);
        return false;
    }
}

void lfo_housekeep() {
    if(user_adjusting()) {
        //Serial.println(adj);
        int data[128];
        
        lfo_set_waveform();

        ui.clearDisplay();
        lfo_print_param();
        //   ui.printText("LFO " + String(lfo_period.get()) + "ms",0,0,2);
        for(int i=0;i<128;i++) {
            float val = float(wave_table[i]) / 1024.0;
            data[i] = 63 - ( int(48*val));
            //data[i] = 40+24*(cos(i/12.566)+.0001*(sin(i/1.28)));
        }
        ui.graphData(data);
        ui.showDisplay();
        init_adj=adj;
    }
}

void lfo_do_trigger() {
    init_adj=adj;
    boolean adj_changed = false;
    for(int i=0;(i<LFO_PARTS)&&!keypress&&!e.getEncoderValue()&&!adj_changed;i++) {
        //val = (sin(6.28*i/LFO_PARTS)*511)+512;
        analogWrite(A0,wave_table[i]);
        //write_dac(wave_table[i]);
        delayMicroseconds(delay_usecs);
        //adj_changed = abs(adj-init_adj)>20;
        //adj_changed = user_adjusting();
    }
    //lfo_housekeep();
    //Serial.println("Adj changed: "+String(adj_changed));
    // if(adj_changed) lfo_fxn();
    doing_trigger = false;
}

uint16_t lfo_get_param() {
    return lfo_period.get();
}

void lfo_put_param(uint16_t val) {
    lfo_period.put(val);
    delay_usecs = int(993.982*(lfo_period.get() - LFO_BASE_PERIOD)/LFO_PARTS);
    lfo_print_param();
}

void lfo_adjust_param(int e, unsigned long delta) {
    //float multiplier = 10000.0/LFO_PARTS;
    float multiplier = 10;
    if(delta < 350) {
        multiplier = 1000;
    } else {
        if(delta<500) {
            multiplier = 100;
        }
    }
    uint16_t param = lfo_get_param();   // period in ms
    param += e*multiplier;
    lfo_put_param(param);
    // lfo_period.put(param);
    // delay_usecs = int(993.982*(lfo_period.get() - LFO_BASE_PERIOD)/LFO_PARTS);
    // // delay_usecs = int(924.0*lfo_period.get()/LFO_PARTS);
    // lfo_print_param();
}

void lfo_fxn() {
  disable_trigger(false);
  int data[128];
  
  lfo_set_waveform();

  ui.clearDisplay();
  lfo_print_param();
//   ui.printText("LFO " + String(lfo_period.get()) + "ms",0,0,2);
  for(int i=0;i<128;i++) {
    float val = float(wave_table[i]) / 1024.0;
    data[i] = 63 - ( int(48*val));
    //data[i] = 40+24*(cos(i/12.566)+.0001*(sin(i/1.28)));
  }
  ui.graphData(data);
  ui.showDisplay();
}

void lfo_begin() {
    Serial.println("LFO beginning");
    lfo_waveform_num.begin(false);
    lfo_waveform_num.xfer();
    lfo_period.begin(false);
    lfo_period.xfer();
    // lfo_param_num = 0;
    lfo_set_waveform();
    lfo_adjust_param(0,1000); // set init delay value
}



