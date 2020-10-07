EEPROM_Int dvm_mode = EEPROM_Int(0, 1); // 0-DC 1=AC
byte dvm_ctr=0;
double meas_buf[10];
String dvm_meas="";

String dvm_params_toJSON() {
    String out = "";
    out += "{ ";
    out += toJSON("label","Mode");
    out += ", ";
    out += toJSON("type","radio");
    out += ", ";
    out += toJSON("values","DC,AC");
    out += ", ";
    out += toJSON("value",dvm_mode.get() ? "AC" : "DC");
    out += " }";
    return "["+out+"]";
}

void dvm_inc_param_num_by(int val) {
    dvm_mode.inc(val);
    int mode = dvm_mode.get();
    // Serial.print("Mode: :");
    // Serial.println(mode);
    if(mode<0) {
        dvm_mode.put(1);
    } else {
        if(mode>1) dvm_mode.put(0);
    }
    switch(dvm_mode.get()) {
        case 0:
            ui.printText("DC Volts",0,0,2);
            break;
        case 1:
            ui.printText("AC Volts",0,0,2);
            break;
        default:
            ui.printText("Unk",0,0,2);
    }
}

void dvm_do_meas(int ival) {
    float corr = 101.088;
    //Serial.println(analogRead(ain2_pin));
    char buf[10] = {0,0,0,0,0,0,0,0,0,0}; // buffer to print up to 9 digits
    double val = float(ival)/corr;
    meas_buf[dvm_ctr++]=val;
    if(dvm_ctr==10) {
        val=0.0;
        for(dvm_ctr=0;dvm_ctr<10;dvm_ctr++) {
            val+=meas_buf[dvm_ctr];
        }
        val = val/10.0;
        switch(dvm_mode.get()) {
            case 0:
                snprintf(buf, 5, "%+f.1", val);
                break;
            case 1:
                float abs_val = abs(val);
                val = 0.0;
                for(dvm_ctr=0;dvm_ctr<10;dvm_ctr++) {
                    //val+=abs(abs(meas_buf[dvm_ctr])-abs_val);
                    val+=pow(meas_buf[dvm_ctr],2.0);
                }
                val = val/10.0;
                val = sqrt(val);
                snprintf(buf, 4, "%f.1", val);
                break;
        }
        //Serial.println(buf);
        dvm_meas = String(buf);
        ui.printLine(String(buf),20,4);
        dvm_ctr=0;
    }
}

void dvm_do_trigger() {
    byte pin = dvm_mode.get()==0 ? ain2_pin : ain3_pin;
    // Serial.println("Reading pin: "+String(pin));
    //String format = "%"+String(digits)+"d";
    int ival = 512-analogRead(pin);
    // delay(1);
    // //int ival = 212;
    // Serial.println((ival));
    // return;
    if(ival==-511 || ival==512) {
        // overload!
        String ovl = ival==512 ? "++++" : "----";
        ui.printLine(ovl,20,4);
    } else {
        if(repeat_on.get()) {
            dvm_do_meas(ival);
        } else {
            for(int i=0;i<10;i++) {
                dvm_do_meas(ival);
            }
        }
    }
}

void dvm_fxn() {
  ui.clearDisplay();
  //ui.printText("DVM",0,0,2);
  dvm_inc_param_num_by(0);
  //triggered = doing_trigger = true;
  reset_trigger();
  //disable_trigger(true);
}

void dvm_begin() {
    dvm_mode.begin(false);
    dvm_mode.xfer();
}

