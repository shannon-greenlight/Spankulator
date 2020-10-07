#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>
#include "SPANK_ui.h"

#include "SPANK_fxn.h"

SPANK_fxn::SPANK_fxn(String _name,String *_labels, uint16_t ** _params, uint16_t _num_params, SPANK_ui *_ui):params(*_params,_num_params) {
    name = _name;
    labels = _labels;
    ui = _ui;
    num_params = _num_params;
    param_num = 0;
    digit_num = 0;
    params = EEPROM_Arr16(_params[0], _num_params);
    mins = _params[1];
    maxs = _params[2];
    display_fxn = &SPANK_fxn::default_display;
    ui_lines[0]=LINE_1;
    ui_lines[1]=LINE_2;
    ui_lines[2]=LINE_3;
}

void SPANK_fxn::begin() {
    params.begin(false);
    params.xfer();
    // Serial.println("");
    // Serial.print(name + " Beginning "+labels[0]);
    // Serial.println(params.get(0));
}

int SPANK_fxn::offset() {
    return params.offset;
}

uint16_t SPANK_fxn::get_param(int16_t indx) {
    if(indx==-1) indx = param_num;
    return params.get(indx);
    //return params[param_num];
}

uint16_t SPANK_fxn::get_min(int16_t indx) {
    if(indx==-1) indx = param_num;
    return mins[indx];
}

uint16_t SPANK_fxn::get_max(int16_t indx) {
    if(indx==-1) indx = param_num;
    return maxs[indx];
}

void SPANK_fxn::adjust_param(int e, unsigned long delta) {
    uint16_t the_param = get_param();
    int multiplier = 1;
    uint8_t ndigs = get_num_digits(param_num);
    int dig = (ndigs-digit_num)-1;
    // Serial.println("mult: "+String(dig));
    multiplier = pow(10,dig);

    the_param += e*multiplier;

    put_param(the_param);
}

int SPANK_fxn::check_param(int param) {
    // Serial.println("Param OK param#: "+ String(param_num));
    switch(param_num) {
      case LONGEST_PULSE:
        param = max(param,get_param(SHORTEST_PULSE)+1);
        break;
      case SHORTEST_PULSE:
        param = min(param,get_param(LONGEST_PULSE)-1);
        break;

    }
    return param;
}

// TODO fix data type to allow negative numbers
void SPANK_fxn::put_param(uint16_t val ) {
    if(check_params) val=check_param(val);
    int16_t val1 = val;    // deals with 'negative' numbers 
    // Serial.print("Put param: ");
    // Serial.println(val1);
    if(val>get_max()) val = get_min();
    if(val1<get_min()) val = get_max();
    // Serial.print("final val: ");
    // Serial.println(val);
    params.put(val,param_num);
    // (*ui).printParam(get_label(param_num),get_param(param_num),4,"%-4d",ui_lines[param_num]);
    // String fmt = "%0"+String(ndigs)+"d";
    // const char* format = fmt.c_str();
    uint8_t ndigs = get_num_digits(param_num);
    // String fmt = "%0"+String(ndigs)+"d";
    // const char* format = fmt.c_str();
    // const char* format = calc_format(param_num);
    String fmt = calc_format(ndigs);
    const char* format = fmt.c_str();
    (*ui).printParam(get_label(param_num),get_param(param_num),ndigs,format,min(2,param_num));
    //printParams();
}

void SPANK_fxn::hilight_param(void) {
    uint8_t dig_offset = get_label(param_num).length();
    int char_num = digit_num+dig_offset;
    (*ui).underline_char(char_num,min(2,param_num),1,8);
    // Serial.print ("Dig offset: "+String(dig_offset));
    // Serial.print (" Digit #: "+String(digit_num));
    // Serial.println(" Char Num: "+String(char_num));
}

void SPANK_fxn::inc_param_num_by(int val) {
    param_num += val;
    if(param_num<0) param_num = num_params;
    if(param_num>num_params-1) param_num = 0;
    digit_num = 0;
    printParams();
    // hilight_param();
}

void SPANK_fxn::inc_dig_num_by(int val) {
    // uint8_t num_digs = log10(get_max(param_num))+1;
    uint16_t num_digs = get_num_digits(param_num);
    digit_num += val;
    if(digit_num<0) digit_num = 0;
    if(digit_num>num_digs-1) digit_num = 0;
    // Serial.println("Dig# "+String(digit_num)+" Digs "+String(num_digs));
    printParams();
    // hilight_param();
}

String SPANK_fxn::get_label(int label_num) {
    return labels[label_num];
}

void SPANK_fxn::printParams() {
    (*ui).fill(BLACK,16);
    hilight_param();
    // Serial.println("Num params: "+String(num_params) + " Param# "+String(param_num));
    int start = max(0,param_num-2);
    int end = min(3,num_params);
    for(int i=0;i<end;i++) {
        uint8_t ndigs = get_num_digits(i+start);
        // String fmt = "%0"+String(ndigs)+"d";
        //  const char* format = fmt.c_str();
        String fmt = calc_format(ndigs);
        const char* format = fmt.c_str();
        //param_num = i;
        // (*ui).printParam(get_label(i),get_param(i),4,"%-4d",ui_lines[i]);
        // Serial.println("Item: "+String(get_label(i+start)) + " Start: "+String(i+start) + " Ndigs: " + String(ndigs));
        // (*ui).printParam(get_label(i+start),get_param(i+start),ndigs,"%04d",i);
        (*ui).printParam(get_label(i+start),get_param(i+start),ndigs,format,i);
    }
}

String SPANK_fxn::toJSON(String key, String value) {
  return "\""+key+"\" :\""+value+"\"";
}

String SPANK_fxn::params_toJSON() {
  String out = "";
  for (int i = 0; i < num_params; i++)
  {
    if(out>"") out += ",";
    out += "{ ";
    out += toJSON("label",get_label(i));
    out += ", ";
    out += toJSON("type","number");
    out += ", ";
    out += toJSON("value",String(get_param(i)));
    out += ", ";
    out += toJSON("param_num",String(i));
    out += ", ";
    out += toJSON("min",String(get_min(i)));
    out += ", ";
    out += toJSON("max",String(get_max(i)));
    out += ", ";
    out += toJSON("selected",param_num==i ? "true" : "false" );
    out += " }";
  }
  return "["+out+"]";
}


void SPANK_fxn::default_display() {
//   Serial.println("Name: "+name);
//   (*ui).clearDisplay();
//   (*ui).printLine(name,0,2);
  (*ui).newFxn(name);
  printParams();
}

void SPANK_fxn::display() {
    (this->*display_fxn)();
}

void SPANK_fxn::copy_to(SPANK_fxn &target) {
    for(int i=0;i<num_params;i++) {
        target.params.put(get_param(i),i);
        target.labels[i]=get_label(i);
        target.mins[i]=get_min(i);
        target.maxs[i]=get_max(i);
    }
    target.name=name;
    target.trigger_fxn = trigger_fxn;
    target.num_params=num_params;
    // Serial.println("Copying: "+target.name);
}

void SPANK_fxn::debug() {
  Serial.println("Howdy I'm "+name);
  Serial.println("Initial Delay: "+String(get_param(INITIAL_DELAY)));
}





// private fxns
uint8_t SPANK_fxn::get_num_digits(int this_param_num) {
    // Serial.println("Getting ndigs: "+String(this_param_num));
    return log10(get_max(this_param_num))+1;
}

String SPANK_fxn::calc_format(uint8_t ndigs) {
        // uint8_t ndigs = get_num_digits(indx);
        return "%0"+String(ndigs)+"d";
        // const char* format = fmt.c_str();
        // return format;
}
