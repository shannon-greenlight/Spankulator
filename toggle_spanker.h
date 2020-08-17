uint16_t _toggle_params[] = {0};
uint16_t _toggle_mins[] = {0};
uint16_t _toggle_maxs[] = {9999};
uint16_t *toggle_stuff[] = {_toggle_params, _toggle_mins, _toggle_maxs};
String toggle_labels[] = {"Delay: "};
SPANK_fxn toggle_spanker("Toggle", toggle_labels, toggle_stuff, sizeof(_toggle_params)/sizeof(_toggle_params[0]), &ui);

void toggle_trigger() {
  reset_trigger();
  triggered=true;
  doing_trigger=true;
  int delay = toggle_spanker.get_param(0);
  Serial.print("Toggle Trigger, delay: ");
  Serial.println(delay);
  int aval1 = delay%1023;
  write_dac(aval1);
  do_delay(delay);
  do_toggle();
  reset_trigger();
  digitalWrite(gate_out_pin, toggle_state ? HIGH : LOW);
  doing_trigger=false;
}

void toggle_fxn() {
  the_spanker = &toggle_spanker;
  toggle_state = true;
  do_toggle();
  digitalWrite(gate_out_pin, LOW);
  toggle_spanker.display();
}

void toggle_begin() {
  toggle_spanker.begin();
  toggle_spanker.trigger_fxn = toggle_trigger;
}


