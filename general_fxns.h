// declarations for later
String fxn_name(void);
String list_fxns(void);
void process_cmd(String);
void process_keypress(void);
void put_param_num(uint16_t pnum);
void exe_fxn(void);

// fxn definitions
boolean isDifferent(float a, float b) {
	int ai = int(100 * a);
	int bi = int(100 * b);
	return (ai != bi);
}

String enquote(String value) {
  return "\""+value+"\"";
}

String toJSON(String key, String value) {
  return "\""+key+"\" :\""+value+"\"";
}

// hardware primitives
boolean read_trigger_pin() {
	return !digitalRead(trigger_button_pin);
}

boolean all_buttons_up()
{
  //Serial.println(digitalRead(trigger_button_pin));
  return digitalRead(up_button_pin) &&
         digitalRead(dn_button_pin) &&
         digitalRead(repeat_button_pin) &&
         digitalRead(trigger_button_pin) &&
         digitalRead(param_up_button_pin) &&
         digitalRead(param_dn_button_pin) &&
         digitalRead(left_button_pin) &&
         digitalRead(right_button_pin);
}

void reset_trigger() {
  byte pin = 11;
  if(OLD_STYLE) {
    pin=gate_out_pin;
    digitalWrite(pin, HIGH);
    delay(1);
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, LOW);
    delay(1);
    digitalWrite(pin, HIGH);
  }
  // Serial.print("Resetting trigger. Pin# ");
  // Serial.println(pin);
}

void disable_trigger(boolean disable) {
	//digitalWrite(disable_trigger_pin,disable);
}

void do_toggle() {
  toggle_state = !toggle_state;
  digitalWrite(toggle_out_pin, toggle_state);
}

// void write_dac(uint16_t val) {
//   val = ( val << 2 ) | 0x3000;
//   digitalWrite(dac_select_pin, LOW); //initialize the chip
//   SPI.transfer16(val); // write to device
//   digitalWrite(dac_select_pin, HIGH); // de-initialize the chip
// }

void write_dac(uint16_t val) {
  analogWrite(aout_pin,val);
}

boolean event_pending = false;
void do_delay(unsigned int d) {
  // Serial.println("");
  // Serial.println("Do delay: "+String(d)+" millis: " + String(millis()));
	if(d<200) {
		delay(d);
    // Serial.println("Delay done at: " + String(millis()));
	} else {
		boolean ser_avail, adjusting;
		unsigned long targetMillis = millis() + d;
		do {
      int encoder_val = e.getEncoderValue();
      adjusting = encoder_val != 0 && encoder_val != last_encoder_val;
			ser_avail = false && Serial.available() > 0;
      event_pending = adjusting || ser_avail || keypress;
			// Serial.println("millis: " + String(millis()));
		} while(millis()<targetMillis && !event_pending && triggered);
	}
}

unsigned int calc_delay(unsigned int d, int rnd) {
  float delay_adj = rnd*d/200.0;
  // reuse rnd here
  rnd = random(int(-delay_adj),int(delay_adj));
  return d+rnd;	
}

void do_pulse(unsigned int my_delay, int aval1, int aval2) {
  write_dac(aval1);
  digitalWrite(gate_out_pin, HIGH);
  do_delay(my_delay);
  digitalWrite(gate_out_pin, LOW);
  write_dac(aval2);
  do_delay(my_delay);
}

void scale_and_offset(int* val) {
  *val *= scale;
  *val += offset;
}

void send_one_pulse(float longest_pulse, float shortest_pulse, int rnd, float the_swell) {
  unsigned int my_delay = calc_delay(the_delay,rnd);
  int aval1 = max(0,my_delay*1023/(longest_pulse));
  int aval2 = max(0,(my_delay+the_swell/2)*1023/(longest_pulse));
  aval1 = min(1023,aval1);
  aval2 = min(1023,aval2);
  //calc_scale_and_offset();
  scale_and_offset(&aval1);
  scale_and_offset(&aval2);
  // aval1 *= scale;
  // aval1 += offset;
  // aval2 *= scale;
  // aval2 += offset;
  // Serial.print("Scale: ");
  // Serial.println(scale);
  // Serial.println("Delay: "+String(the_delay)+" My Delay: "+String(my_delay)+" Swell: "+String(the_swell));
  do_pulse(my_delay,aval1,aval2);
}







