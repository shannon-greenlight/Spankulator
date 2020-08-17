uint16_t _dn_params[] = {0, 0, 0, 0, 0};
uint16_t _dn_mins[] = {10, 10, 2, 0, 0};
uint16_t _dn_maxs[] = {9999, 9998, 99, 99, 9999};
uint16_t *dn_stuff[] = {_dn_params, _dn_mins, _dn_maxs};
String dn_labels[] = {"Longest Pulse: ","Shortest Pulse: ", "Num Pulses: ", "Randomness: ", "Initial Delay: "};
SPANK_fxn dn_spanker("Down", dn_labels, dn_stuff, sizeof(_dn_params)/sizeof(_dn_params[0]), &ui);

void dn_trigger() {
  Serial.println("Doing dn trigger");
  float longest_pulse = float(the_spanker->get_param(LONGEST_PULSE));
  float shortest_pulse = the_spanker->get_param(SHORTEST_PULSE);
  float the_swell = (shortest_pulse-longest_pulse)/the_spanker->get_param(NUM_PULSES);
  int rnd = the_spanker->get_param(RANDOMNESS);
  if(!doing_trigger) {
    doing_trigger=true;
    the_delay = longest_pulse;
    do_delay(the_spanker->get_param(INITIAL_DELAY));
  }
  send_one_pulse(longest_pulse, shortest_pulse, rnd, the_swell);
  the_delay += the_swell;
  if(the_delay<=shortest_pulse) { 
    doing_trigger = false;
    the_delay = longest_pulse;
    do_toggle();
  }
}

void dn_fxn() {
  the_spanker = &dn_spanker;
  dn_spanker.display();
}

void dn_begin() {
  dn_spanker.begin();
  dn_spanker.trigger_fxn = dn_trigger;
  dn_spanker.check_params_fxn = check_param;
}

