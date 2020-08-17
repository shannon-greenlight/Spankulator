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
  case DVM_FXN:
    return "DVM";
    break;
  case USER_FXN:
    return "User";
    break;
  case LFO_FXN:
    return "LFO";
    break;
  case WIFI_FXN:
    return "WiFi";
    break;
  default:
    return the_spanker->name;
    //return "Unknown - Fxn: " + String(fxn.get());
  }
}

// trigger
void do_trigger()
{
  digitalWrite(triggered_led_pin,HIGH);
  //reset_trigger();
  switch (fxn.get())
  {
  case LFO_FXN:
    do_toggle();
    digitalWrite(gate_out_pin, HIGH);
    lfo_do_trigger();
    digitalWrite(gate_out_pin, LOW);
    break;
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
  Serial.println("Testing trigger");
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
  default:
    the_spanker->adjust_param(encoder_val, delta);
    //Serial.println("Unknown adjust_param fxn: " + String(fxn.get()));
  }
  //Serial.println("Changed val: "+String(the_param) + " Fxn: " + String(fxn.get()));
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
  case LFO_FXN:
    return lfo_get_param();
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
  case LFO_FXN:
    return lfo_num_params;
    break;
  default:
    return the_spanker->num_params;
    // Serial.println("Unknown num_params fxn: " + String(fxn.get()));
    // return 0;
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
  case LFO_FXN:
    return "LFO";
    break;
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
    //Serial.println("Unknown put_param fxn: " + String(fxn.get()));
  }
}

String get_selected_param()
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    return "WiFi";
    break;
  case DVM_FXN:
    return "DVM";
    break;
  case USER_FXN:
    return "Sequence";
    break;
  case LFO_FXN:
    return "LFO";
    break;
  default:
    return the_spanker->get_label(the_spanker->param_num);
    // Serial.println("Unknown get_selected_param fxn: " + String(fxn.get()));
    // return "?";
  }
}

void inc_dig_num()
{
  switch (fxn.get())
  {
  case WIFI_FXN:
    wifi_inc_param_num_by(1);
    exe_fxn();
    the_param = get_param(param_num);
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
  //the_param = get_param(param_num);
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
    // wifi_inc_param_num_by(1);
    // exe_fxn();
    //param_num=pnum;
    break;
  case DVM_FXN:
    // dvm_inc_param_num_by(1);
    //exe_fxn();
    break;
  case LFO_FXN:
    // lfo_inc_param_num_by(1);
    // exe_fxn();
    break;
  default:
    the_spanker->param_num=pnum;
    //Serial.println("Unknown put_param_num fxn: " + String(fxn.get()));
  }
  //the_param = get_param(param_num);
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
  case LFO_FXN:
    lfo_inc_param_num_by(1);
    exe_fxn();
    break;
  default:
    the_spanker->inc_param_num_by(1);
    //Serial.println("Unknown inc_param_num fxn: " + String(fxn.get()));
  }
  //the_param = get_param(param_num);
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
  case LFO_FXN:
    lfo_inc_param_num_by(-1);
    exe_fxn();
    break;
  default:
    the_spanker->inc_param_num_by(-1);
    //Serial.println("Unknown dec_param_num fxn: " + String(fxn.get()));
  }
  the_param = get_param(param_num);
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
    // arr[0]=toJSON("Sequence", user_string.get());
    // arr[1]=toJSON("U", "- Add UP sput");
    // arr[2]=toJSON("P", "- Add Peck");
    user_get_params(arr);
    Serial.println(arr[7]);
    //Serial.println(arr[1]);
    break;
  default:
    for (int i = 0; i < get_num_params(); i++)
    {
      arr[i] = toJSON(get_label(i), String(get_param(i)));
      Serial.println(arr[i]);
    }
  }
  //String arr[10];  // hopefully 10 will be enough
}

void new_fxn()
{
  // select_wifi_screen = param_num = numSsid = 0;
  //if(wifi_status == WL_CONNECTED) select_wifi_screen = 4;
  select_wifi_screen = (wifi_status == WL_CONNECTED) ? 4 : 0;

  param_num = numSsid = 0;
  triggered = doing_trigger = user_doing_trigger = false;
  reset_trigger();
  event_pending = false;
  exe_fxn();
  the_param = get_param(param_num);
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
  Serial.print("Processing keypress: ");
  Serial.println(char(keypress));
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
  case 42: // *
    repeat_on.toggle();
    digitalWrite(repeat_led_pin, repeat_on.get());
    // Serial.println("SSID: " + wifi_ssid.get() + " Scrn# " + String(select_wifi_screen));
    // wifi_ssid.test();
    //wifi_password.test();
    //wifi_active.test();
    break;
  case 33: // !
    triggered = !triggered;
    Serial.println("Triggered: " + String(triggered) + " Repeat: " + String(repeat_on.get()));
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

  the_param = get_param(param_num);

}
