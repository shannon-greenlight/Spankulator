#define USER_MAXLEN 10
#define USER_MAX_PARAMS 5

String user_get_sput(char);

EEPROM_String user_string(USER_MAXLEN);
//EEPROM_Arr16 *user_params[USER_MAXLEN];

uint16_t user_params_0[USER_MAX_PARAMS];
uint16_t user_params_1[USER_MAX_PARAMS];
uint16_t user_params_2[USER_MAX_PARAMS];
uint16_t user_params_3[USER_MAX_PARAMS];
uint16_t user_params_4[USER_MAX_PARAMS];
uint16_t user_params_5[USER_MAX_PARAMS];
uint16_t user_params_6[USER_MAX_PARAMS];
uint16_t user_params_7[USER_MAX_PARAMS];
uint16_t user_params_8[USER_MAX_PARAMS];
uint16_t user_params_9[USER_MAX_PARAMS];

String user_labels[USER_MAXLEN];
uint16_t user_maxs[USER_MAXLEN];
uint16_t user_mins[USER_MAXLEN];

uint16_t *user_stuff_0[] = {user_params_0, user_mins, user_maxs};
uint16_t *user_stuff_1[] = {user_params_1, user_mins, user_maxs};
uint16_t *user_stuff_2[] = {user_params_2, user_mins, user_maxs};
uint16_t *user_stuff_3[] = {user_params_3, user_mins, user_maxs};
uint16_t *user_stuff_4[] = {user_params_4, user_mins, user_maxs};
uint16_t *user_stuff_5[] = {user_params_5, user_mins, user_maxs};
uint16_t *user_stuff_6[] = {user_params_6, user_mins, user_maxs};
uint16_t *user_stuff_7[] = {user_params_7, user_mins, user_maxs};
uint16_t *user_stuff_8[] = {user_params_8, user_mins, user_maxs};
uint16_t *user_stuff_9[] = {user_params_9, user_mins, user_maxs};

SPANK_fxn user_spanker_0("User 0", user_labels, user_stuff_0, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_1("User 1", user_labels, user_stuff_1, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_2("User 2", user_labels, user_stuff_2, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_3("User 3", user_labels, user_stuff_3, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_4("User 4", user_labels, user_stuff_4, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_5("User 5", user_labels, user_stuff_5, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_6("User 6", user_labels, user_stuff_6, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_7("User 7", user_labels, user_stuff_7, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_8("User 8", user_labels, user_stuff_8, USER_MAX_PARAMS, &ui);
SPANK_fxn user_spanker_9("User 9", user_labels, user_stuff_9, USER_MAX_PARAMS, &ui);

SPANK_fxn *user_spanks[] = { &user_spanker_0, &user_spanker_1, &user_spanker_2, &user_spanker_3, &user_spanker_4, &user_spanker_5, &user_spanker_6, &user_spanker_7, &user_spanker_8, &user_spanker_9 };

byte user_dig_num = 0;
byte user_op_index = 0;
String user_ops = "UDTBCS";
byte user_num_params = user_ops.length()+1;
boolean user_doing_trigger = false;

void user_set_params(char c);
void user_display();

void debug() {
  Serial.println("Howdy User Fxn!");
  user_dig_num++;
  if(user_dig_num>=4) user_doing_trigger=doing_trigger=false;
}

// not used
void user_get_params(String arr[]) {
  char c;
  arr[0]=toJSON("Sequence", user_string.get());
  for(int i=0;i<user_num_params;i++) {
    c = user_ops[i];
    arr[i+1]=toJSON(String(c), user_get_sput(c));
    //Serial.println(String(c+" "+user_get_sput(c)));
  }
}

void user_update_user_string(char cmd, String in_str) {
  if(user_ops.indexOf(cmd)!=-1) {
    triggered = user_doing_trigger = false;
    user_string.put(in_str);
    for(user_dig_num=0;user_dig_num<in_str.length();user_dig_num++) {
      user_set_params(user_string.charAt(user_dig_num));    
    }
    user_dig_num=0;
    user_display();
  }
}

SPANK_fxn user_get_spank_obj(char c) {
  SPANK_fxn s = dn_spanker;
  switch(c) {
    case 'U':
      s = up_spanker;
      break;
    case 'D':
      s = dn_spanker;
      break;
    case 'C':
      s = stretch_spanker;
      break;
    case 'S':
      s = maytag_spanker;
      break;
    case 'T':
      s = toggle_spanker;
      break;
    case 'B':
      s = toggle_spanker;
      break;
  }
  return s;
}

String user_get_sput(char c) {
  String s = "Down";
  switch(c) {
    case 'U':  
      s = "Up";
      break;
    case 'D':  
      s = "Down";
      break;
    case 'T':  
      s = "Top (Toggle HIGH)";
      break;
    case 'B':  
      s = "Bottom (Toggle LOW)";
      break;
    case 'C':  
      s = "Charm (Stretch Mode)";
      break;
    case 'S':  
      s = "Strange (Maytag Mode)";
      break;
  }
  return s;
}

String user_params_toJSON() {
  //SPANK_fxn s = user_get_spank_obj(user_string.charAt(user_dig_num));
  //SPANK_fxn s = *user_spanks[user_dig_num];
  //s.param_num=9999; // this prevents param from being selected
  // Serial.println("Params to JSON -- Spanker: " + user_spanks[user_dig_num]->name + " param_num: " + String(user_spanks[user_dig_num]->param_num));
  return user_spanks[user_dig_num]->params_toJSON();
}

void user_begin() {
  //user_spanker_0.trigger_fxn=debug;
  user_string.begin(false);
  user_string.xfer();
  for(int i=0;i<USER_MAXLEN;i++) {
    the_spanker = user_spanks[i];
    the_spanker->begin();
    //user_set_params(user_string.charAt(i));
    //*user_spanks[i].xfer();
    SPANK_fxn f = user_get_spank_obj(user_string.charAt(i));
    if(f.name > "") {
      // Serial.println("User Begin: "+user_spanks[i]->name);
      user_spanks[i]->trigger_fxn=f.trigger_fxn;
      for(int j=0;j<f.num_params;j++) {
        user_spanks[i]->labels[j]=f.labels[j];
        user_spanks[i]->mins[j]=f.get_min(j);
        user_spanks[i]->maxs[j]=f.get_max(j);
        //Serial.println("Setting min: "+String(j)+ " val: "+String(f.get_min(j))+ f.name);
      }
      //user_spanks[i]->mins[i]=f.get_min(i);
    }
  }
  //*user_spanks[0].trigger_fxn=up_trigger;
}

void user_display() {
  ui.printLine(user_string.get(),lines[0],2);
  ui.underline_char(user_dig_num,1);  // min(9,user_dig_num)
  String op = user_get_sput(user_string.charAt(user_dig_num));
  ui.printLine(op,lines[2],1);
}

void user_fxn() {
  user_doing_trigger = false;
  ui.newFxn("User");
  // ui.clearDisplay();
  // ui.printText("User",0,0,2);
  //user_string.put("UDU      ");
  //user_string.put("DUSDC     ");
  user_display();
}

void user_inc_dig_num_by(byte n) {
  user_dig_num += n;
  user_dig_num = max(0, user_dig_num);
  if(user_dig_num > USER_MAXLEN) user_dig_num = 0;
  user_display();
  // Serial.print("User Inc Dig Num: ");
  // Serial.println(user_dig_num);
}

void user_home() {
  user_dig_num = 0;
  user_display();
  //ui.underline_char(user_dig_num,1);
}

void user_end() {
  user_dig_num = 0;
  do {
    user_dig_num++;
  } while (user_string.charAt(user_dig_num) != ' ' && user_dig_num<USER_MAXLEN);
  user_display();
  //ui.underline_char(user_dig_num,1);
}

void user_set_params(char c) {
  SPANK_fxn f = user_get_spank_obj(c);
  SPANK_fxn g = *user_spanks[user_dig_num];
  f.copy_to(g);
  // (&g)->trigger_fxn=debug;
  // (&g)->name="Sammy";
  //Serial.println(g.name);
}

void user_put_param_num(uint16_t pnum) {
  user_spanks[user_dig_num]->param_num=pnum;
  user_display();
  // Serial.println("User put param num: " + user_spanks[user_dig_num]->name);
  // Serial.println("!param_num: " + String(user_spanks[user_dig_num]->param_num));
}

void user_put_param(int param) {
  //SPANK_fxn g = *user_spanks[user_dig_num];
  // Serial.println("User put param: " + user_spanks[user_dig_num]->name);
  // Serial.println("param_num: " + String(user_spanks[user_dig_num]->param_num));
  // Serial.println("max: " + String(user_spanks[user_dig_num]->get_max()));
  // Serial.println("min: " + String(user_spanks[user_dig_num]->get_min()));
  user_spanks[user_dig_num]->put_param(param);
  ui.clearDisplay();
  ui.printText("User",0,0,2);
  user_display();
}

void user_adjust_param(int encoder_val) {
  char c = user_ops[user_op_index];
  // Serial.print("User Adjust Param: ");
  // Serial.println(user_string.length());
  // Serial.println(user_dig_num);
  user_op_index += encoder_val;
  if(user_op_index<0) user_op_index = user_ops.length()-1;
  if(user_op_index>=user_ops.length()) user_op_index = 0;
  if(user_string.length()>user_dig_num) {
    user_string.putCharAt(c,user_dig_num);
  } else {
    user_string.append(String(c));
  }
  user_set_params(c);
  // Serial.println(user_spanks[user_dig_num]->get_label(0));
  user_display();
}

void user_do_trigger() {
  // Serial.println("Doing user trigger!");
  if(!user_doing_trigger) {
    // initialize  ptr and set flag
    user_doing_trigger = true;
    user_dig_num = 0;
    //user_display();
    ui.underline_char(user_dig_num,1);
    // the_spanker->debug();
  }
  the_spanker = user_spanks[user_dig_num];
  //Serial.println(the_spanker->name+ " doing trigger at: "+String(millis()));
  //user_display();
  //SPANK_fxn f = user_get_spank_obj(user_string.charAt(user_dig_num));
  //Serial.println(f.name);
  //Serial.println(String(*f.trigger_fxn));
  //f.trigger_fxn();
  the_spanker->trigger_fxn();
  if(!doing_trigger) {
    do {
      user_dig_num++;
    } while(user_string.charAt(user_dig_num)==' ');
    if(user_dig_num>=user_string.length()) {
      user_doing_trigger = doing_trigger = false;
    } else {
      // the_spanker->debug();
      //user_display();
      ui.underline_char(user_dig_num,1);
    }
  }
}

