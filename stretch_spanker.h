uint16_t _stretch_params[] = {0, 0, 0};
uint16_t _stretch_mins[] = {1, 0, 1};
uint16_t _stretch_maxs[] = {9999, 9999, 99};
uint16_t *stretch_stuff[] = {_stretch_params, _stretch_mins, _stretch_maxs};
String stretch_labels[] = {"Pulse Length: ", "Delay: ", "#Pulses: "};
SPANK_fxn stretch_spanker("Stretch", stretch_labels, stretch_stuff, sizeof(_stretch_params)/sizeof(_stretch_params[0]), &ui);

void do_stretch_pulse(unsigned int delay, unsigned int pulse_len) {
  int aval1 = delay%1023;
  int aval2 = pulse_len%1023;
  write_dac(aval1);
  do_delay(delay);
  digitalWrite(gate_out_pin, HIGH);
  write_dac(aval2);
  do_delay(pulse_len);
  digitalWrite(gate_out_pin, LOW);
}

void stretch_trigger() {
  triggered=true;
  doing_trigger=true;
  int pulse_len = stretch_spanker.get_param(0);
  int delay = stretch_spanker.get_param(1);
  int num_pulses = stretch_spanker.get_param(2);
  // Serial.print("Stretch Trigger, max: ");
  // Serial.println(stretch_spanker.get_max(1));
  for(int i=0;i<num_pulses;i++) {
    do_stretch_pulse(delay,pulse_len);
  }
  doing_trigger=false;
  do_toggle();
}

void stretch_fxn() {
  the_spanker = &stretch_spanker;
  stretch_spanker.display();
}

void stretch_begin() {
  stretch_spanker.begin();
  stretch_spanker.trigger_fxn = stretch_trigger;
}

