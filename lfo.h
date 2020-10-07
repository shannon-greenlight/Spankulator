#include <Wire.h>           // For I2C comm, but needed for not getting compile error
#include <TerminalVT100.h>
#include <stdlib.h>

#define LFO_BASE_PERIOD 2
#define LFO_BASE_DELAY 2
#define LFO_PARTS 128
#define LFO_PERIOD_INDX 0
#define LFO_WAVEFORM_INDX 1

void lfo_fxn();

uint16_t wave_table[128] PROGMEM;
int delay_usecs=39047;

uint16_t _lfo_params[] = {10,0};
uint16_t _lfo_mins[] = {10,0};
uint16_t _lfo_maxs[] = {9999,1};
uint16_t *lfo_stuff[] = {_lfo_params, _lfo_mins, _lfo_maxs};
String lfo_labels[] = {"Period: ","Waveform: "};
SPANK_fxn lfo_spanker("LFO", lfo_labels, lfo_stuff, sizeof(_lfo_params)/sizeof(_lfo_params[0]), &ui);

// void write_dac(uint16_t val) {
//   val = ( val << 2 ) | 0x3000;
//   digitalWrite(dac_select_pin, LOW); //initialize the chip
//   SPI.transfer16(val); // write to device
//   digitalWrite(dac_select_pin, HIGH); // de-initialize the chip
// }

String lfo_waveform_name() {
    return lfo_spanker.get_param(LFO_WAVEFORM_INDX)==0 ? "Sine" : "Ramp";
    //return lfo_waveform_num.get()==0 ? "Sine" : "Ramp";
}

// String lfo_params_toJSON() {
//     String out = "";
//     out += "{ ";
//     out += toJSON("label","Period (ms)");
//     out += ", ";
//     out += toJSON("type","number");
//     out += ", ";
//     out += toJSON("value",String(lfo_period.get()));
//     out += ", ";
//     out += toJSON("min",String(lfo_period.min));
//     out += ", ";
//     out += toJSON("max",String(lfo_period.max));
//     out += ", ";
//     out += toJSON("selected","true");
//     out += " }";
//     out += ",";
//     out += "{ ";
//     out += toJSON("label","Waveform");
//     out += ", ";
//     out += toJSON("type","text");
//     out += ", ";
//     out += toJSON("value",lfo_waveform_name() );
//     out += ", ";
//     out += toJSON("selected","false");
//     out += " }";
//     return "["+out+"]";
// }

void lfo_print_params() {
    ui.terminalPrintParam(DECAY_ROW,"Waveform: ",lfo_waveform_name());
}

void lfo_display() {
  ui.newFxn(lfo_spanker.name);
  lfo_spanker.printParams();
}

void lfo_set_waveform() {
    int temp;
    switch(lfo_spanker.get_param(LFO_WAVEFORM_INDX)) {
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
        Serial.println("LFO set waveform unknown table: "+String(lfo_spanker.get_param(LFO_WAVEFORM_INDX)));
    }
    lfo_print_params();
    //ui.terminalPrintParam(PULSE_LEN_ROW,"Waveform",lfo_waveform_name());
}

void set_adj() {
    adj = analogRead(ain1_pin);
}

boolean user_adjusting() {
    static int cnt = 0;
    if(cnt++>10) {
        //cnt=0;
        set_adj();
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

        ui.fill(BLACK,16);
        //lfo_new_fxn();
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
    do_toggle();
    digitalWrite(gate_out_pin, HIGH);
    set_adj();
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
    digitalWrite(gate_out_pin, LOW);
}

// uint16_t lfo_get_param() {
//     return lfo_period.get();
// }

void lfo_set_params() {
    switch(lfo_spanker.param_num) {
        case LFO_PERIOD_INDX:
            delay_usecs = int(993.982*(lfo_spanker.get_param(LFO_PERIOD_INDX) - LFO_BASE_PERIOD)/LFO_PARTS);
            break;
        case LFO_WAVEFORM_INDX:
            lfo_set_waveform();
            break;
    }
}

void lfo_put_param(uint16_t val) {
    lfo_spanker.put_param(val);
    lfo_set_params();
}

void lfo_adjust_param(int e, unsigned long delta) {
    lfo_spanker.adjust_param(e, delta);
    lfo_put_param(lfo_spanker.get_param());
}

void lfo_fxn() {
    disable_trigger(false);
    the_spanker = &lfo_spanker;
    lfo_set_params();
    set_adj();
    init_adj=adj;
    lfo_display();
}

void lfo_begin() {
    // Serial.println("LFO beginning");
    lfo_spanker.begin();
    lfo_spanker.trigger_fxn = lfo_do_trigger;
    lfo_set_waveform();
    //lfo_adjust_param(0,1000); // set init delay value
}



