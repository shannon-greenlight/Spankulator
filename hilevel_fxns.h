void set_encoder();

FunctionPointer functionPointers[] = {up_fxn, dn_fxn, stretch_fxn, toggle_fxn, maytag_fxn, lfo_fxn, user_fxn, dvm_fxn, select_wifi_fxn, settings_fxn};
int num_fxns = sizeof(functionPointers) / sizeof(functionPointers[0]);
void exe_fxn() {
  disable_trigger(false);
  (*functionPointers[fxn.get()])();
}

String fxn_name()
{
  switch (fxn.get())
  {
  case UP_FXN:
    return up_spanker.name;
    break;
  case DN_FXN:
    return dn_spanker.name;
    break;
  case STRETCH_FXN:
    return stretch_spanker.name;
    break;
  case TOGGLE_FXN:
    return toggle_spanker.name;
    break;
  case MAYTAG_FXN:
    return maytag_spanker.name;
    break;
  case LFO_FXN:
    return lfo_spanker.name;
    break;
  case USER_FXN:
    return "User";
    break;
  case DVM_FXN:
    return "DVM";
    break;
  case WIFI_FXN:
    return "WiFi";
    break;
  case SETTINGS_FXN:
    return "Settings";
    break;
  default:
    //return the_spanker->name;
    return "Unknown - Fxn: " + String(fxn.get());
  }
}

String list_fxns() {
  String out="\"fxns\" : [";
  int fxn_memory = fxn.get();
  for(int i=0;i<num_fxns;i++) {
    fxn.put(i);
    out += enquote(fxn_name());
    // out += toJSON("Fxn_"+String(i),fxn_name());
    if(i<num_fxns-1) out += ",";
  }
  out += "],";
  fxn.put(fxn_memory);
  // Serial.println(out);
  return out;
}

// trigger
void do_trigger()
{
  digitalWrite(triggered_led_pin,HIGH);
  //reset_trigger();
  switch (fxn.get())
  {
  case DVM_FXN:
    dvm_do_trigger();
    break;
  case USER_FXN:
    user_do_trigger();
    break;
  case WIFI_FXN:
    repeat_on.reset();
    select_wifi_ok();
    exe_fxn();
    break;
  default:
    the_spanker->trigger_fxn();
    //Serial.println("Unknown trigger fxn: " + String(fxn.get()));
  }
}

void test_trig()
{
  // Serial.println("Testing trigger");
}

void adjust_param(int encoder_val)
{
  unsigned long now = millis();
  unsigned long delta = now - last_change_time;
  //Serial.println("Encoder: "+String(encoder_val));
  switch (fxn.get())
  {
  case WIFI_FXN:
    wifi_adjust_param(encoder_val);
    break;
  case LFO_FXN:
    lfo_adjust_param(encoder_val, delta);
    break;
  case USER_FXN:
    user_adjust_param(encoder_val);
    break;
  case SETTINGS_FXN:
    the_spanker->adjust_param(encoder_val, delta);
    set_encoder();
    break;
  default:
    the_spanker->adjust_param(encoder_val, delta);
    //Serial.println("Unknown adjust_param fxn: " + String(fxn.get()));
  }
  e.setEncoderValue(0);
  last_encoder_val = encoder_val = 0;
  last_change_time = now;
}

uint16_t get_param(int p_num)
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    return wifi_get_param(p_num);
    break;
  case USER_FXN:
  case DVM_FXN:
  //case LFO_FXN:
    return 0;
    //return lfo_get_param();
    break;
  default:
    return the_spanker->get_param(p_num);
    // Serial.println("Unknown get_param fxn: " + String(fxn.get()));
    // return 0;
  }
}

byte get_num_params()
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    return wifi_num_params;
    break;
  case DVM_FXN:
    return 0;
    break;
  case USER_FXN:
    return user_num_params;
    break;
  default:
    return the_spanker->num_params;
  }
}

String get_label(int i)
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    return "WiFi";
    break;
  case USER_FXN:
    return "User";
    break;
  // case LFO_FXN:
  //   return "LFO";
  //   break;
  default:
    return the_spanker->get_label(i);
    // Serial.println("Unknown get_label fxn: " + String(fxn.get()));
    // return "?";
  }
}

void put_param(int val)
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    break;
  case DVM_FXN:
    break;
  case USER_FXN:
    user_put_param(val);
    break;
  case LFO_FXN:
    lfo_put_param(val);
    break;
  default:
    the_spanker->put_param(val);
  }
}

void inc_dig_num()
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    wifi_inc_param_num_by(1);
    exe_fxn();
    break;
  case USER_FXN:
    user_inc_dig_num_by(1);
    break;
  default:
    the_spanker->inc_dig_num_by(1);
  }
}

void dec_dig_num()
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    wifi_inc_param_num_by(-1);
    exe_fxn();
    break;
  case USER_FXN:
    user_inc_dig_num_by(-1);
    break;
  default:
    the_spanker->inc_dig_num_by(-1);
  }
}

void put_param_num(uint16_t pnum)
{
  switch (fxn.get())
  {
  case USER_FXN:
    user_put_param_num(pnum);
    //user_end();
    break;
  case WIFI_FXN:
    // do nothing
    break;
  case DVM_FXN:
    // dvm_put_param_num(pnum);
    //exe_fxn();
    break;
  default:
    the_spanker->param_num=pnum;
    //Serial.println("Unknown put_param_num fxn: " + String(fxn.get()));
  }
}

void inc_param_num()
{
  switch (fxn.get())
  {
  case USER_FXN:
    user_end();
    break;
  case WIFI_FXN:
    wifi_inc_param_num_by(1);
    exe_fxn();
    break;
  case DVM_FXN:
    dvm_inc_param_num_by(1);
    //exe_fxn();
    break;
  default:
    the_spanker->inc_param_num_by(1);
    //Serial.println("Unknown inc_param_num fxn: " + String(fxn.get()));
  }
}

void dec_param_num()
{
  switch (fxn.get())
  {
  case USER_FXN:
    user_home();
    break;
  case WIFI_FXN:
    wifi_inc_param_num_by(-1);
    exe_fxn();
    break;
  case DVM_FXN:
    dvm_inc_param_num_by(-1);
    //exe_fxn();
    break;
  default:
    the_spanker->inc_param_num_by(-1);
    //Serial.println("Unknown dec_param_num fxn: " + String(fxn.get()));
  }
}

void housekeep() {
  switch (fxn.get())
  {
  case LFO_FXN:
    lfo_housekeep();
    break;    
  }
}

// used by wifi server
void get_params(String arr[])
{
  switch (fxn.get())
  {
  case USER_FXN:
    user_get_params(arr);
    break;
  default:
    for (int i = 0; i < get_num_params(); i++)
    {
      arr[i] = toJSON(get_label(i), String(get_param(i)));
    }
  }
}

void set_encoder() {
  switch(settings_get_encoder_type()) {
    case 0: // bourns
      e.msb_pin=3;
      e.lsb_pin=2;
      break;
    case 1: // amazon
      e.msb_pin=2;
      e.lsb_pin=3;
      break;
  }

  // #define ENCODER_MSB 2
  // #define ENCODER_LSB 3
  // e.msb_pin=ENCODER_MSB;
  // e.lsb_pin=ENCODER_LSB;

  // now set up interrupts
  attachInterrupt(digitalPinToInterrupt(e.lsb_pin), intFxnB, RISING);
  attachInterrupt(digitalPinToInterrupt(e.msb_pin), intFxnA, RISING);

}

void new_fxn()
{
  triggered = doing_trigger = user_doing_trigger = false;
  reset_trigger();
  event_pending = false;
  exe_fxn();
  wifi_new_fxn();
}

//boolean monitor = false;
void heartbeat()
{
  //ui.drawPixel(127, 0, monitor ? WHITE : BLACK);
  // digitalWrite(gate_out_pin,monitor);
  // monitor = !monitor;
  if (!keypress)
  {
    if(fxn.get()!=DVM_FXN && fxn.get()!=LFO_FXN) {
      // read adjustment functionPointers
      adj = analogRead(ain1_pin);
      scale = (adj/1023.0);
      offset = (512-adj/2);
    }

    if (!digitalRead(up_button_pin))
      keypress = 62;
    if (!digitalRead(dn_button_pin))
      keypress = 60;
    if (!digitalRead(repeat_button_pin))
      keypress = 42;
    if (!digitalRead(trigger_button_pin))
      keypress = 33;
    if (!digitalRead(param_up_button_pin))
    {
      esc_mode = true;
      keypress = 65; // up
    }
    if (!digitalRead(param_dn_button_pin))
    {
      esc_mode = true;
      keypress = 66; // dn
    }
    if (!digitalRead(right_button_pin))
    {
      esc_mode = true;
      keypress = 67;
    };
    if (!digitalRead(left_button_pin))
    {
      esc_mode = true;
      keypress = 68;
    };
  }
  else
  {
    if (all_buttons_up())
    {
      keypress = 0;
    }
  }
  if (Serial.available() > 0)
  {
    int c = Serial.read();
    if (c > 20)
    {
      keypress = c;
      //Serial.println(String(Serial.read()));
      //Serial.write(c);
      //Serial.print("(" + String(c) + ")");
    }
  }

  // digitalWrite(gate_out_pin, HIGH);
  // delayMicroseconds(150);
  // digitalWrite(gate_out_pin, LOW);
}

void process_keypress()
{
  // Serial.print("Processing keypress: ");
  // Serial.println(char(keypress));
  switch (keypress)
  {
  case 62: // >
  case 43: // +
    fxn.inc();
    new_fxn();
    break;
  case 60: // <
  case 45: // -
    fxn.inc(-1);
    new_fxn();
    break;
  case 102: // f
    new_fxn();
    break;
  case 42: // *
    repeat_on.toggle();
    digitalWrite(repeat_led_pin, repeat_on.get());
    break;
  case 33: // !
    triggered = !triggered;
    // Serial.println("Triggered: " + String(triggered) + " Repeat: " + String(repeat_on.get()));
    //delay(250);
    break;
  case 65: // up arrow in esc mode A
    if (keypress == 65 && esc_mode)
    {
      inc_param_num();
    }
    break;
  case 66: // dn arrow B
    if (keypress == 66 && esc_mode)
    {
      dec_param_num();
    }
    break;
  case 67: // right arrow in esc mode C
    if (keypress == 67 && esc_mode)
    {
      inc_dig_num();
    }
    break;
  case 68: // left arrow D
    if (keypress == 68 && esc_mode)
    {
      dec_dig_num();
    }
    break;
    // default:
    //   Serial.println("Unknown keypress: "+String(keypress));
  }
  esc_mode = keypress == 27 || keypress == 91;

  // wait for key up
  boolean keyup = false;
  do
  {
    reset_trigger();
    keyup = all_buttons_up();
    //Serial.println("Waiting for keyup: ");
    // Serial.println(keyup);
  } while (!keyup);
  delay(50);
  keypress = 0;
  //Serial.println("Esc Mode: "+String(esc_mode));
}

void process_cmd(String in_str) {
  char cmd = in_str.charAt(0);

  if(cmd=='p') {
    put_param_num(in_str.substring(1).toInt());
    exe_fxn();
  }

  if(cmd=='f') {
    fxn.put(in_str.substring(1).toInt());
  }

  if(fxn.get()==USER_FXN && !esc_mode) {
    user_update_user_string(cmd, in_str);
    // if(cmd>64 && cmd<91) {
    //   user_string.put(in_str);
    //   user_display();
    // }
  }

  if(cmd>47 && cmd<58) {
    //int c = in_str.toInt();
    // Serial.println("Number received: "+in_str);
    put_param(in_str.toInt());
  }

  if(String(cmd) != "\n" && cmd != 112) {
    if(String(cmd)=="%") {
      keypress = 27;
    } else {
      keypress = cmd;
    }
    
    process_keypress();
  }

}

void fxn_begin() {
  fxn.max = num_fxns - 1;
  //fxn.max=7;

  fxn.begin(true);
  repeat_on.begin(false);

  fxn.xfer();
  repeat_on.xfer();
  // fxn.put(0);
  if (fxn.get() == WIFI_FXN) fxn.put(0);

  keypress = 0;
  reset_trigger();
  disable_trigger(false);

  digitalWrite(repeat_led_pin, repeat_on.get());

}

void heartbeat_begin() {
  MyTimer5.begin(5);  // 5 times a sec

  // define the interrupt callback function
  MyTimer5.attachInterrupt(heartbeat);

  // start the timer
  MyTimer5.start();
}

void begin_all() {
  // Start Serial
  Serial.begin(9600);
  //delay(2000);
  //while (! Serial);

  heartbeat_begin();
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
  settings_begin();

  hardware_begin();  
  fxn_begin();
}


