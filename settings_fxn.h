uint16_t _settings_params[] = {0,0};
uint16_t _settings_mins[] = {0,0};
uint16_t _settings_maxs[] = {1,1};
uint16_t *settings_stuff[] = {_settings_params, _settings_mins, _settings_maxs};
String settings_labels[] = {"Ext Clk: ","Encoder Type: "};
SPANK_fxn settings_spanker("Settings", settings_labels, settings_stuff, sizeof(_settings_params)/sizeof(_settings_params[0]), &ui);

void settings_fxn() {
  the_spanker = &settings_spanker;
  settings_spanker.display();
}

uint16_t settings_get_ext_clk() {
  return settings_spanker.get_param(0);
}

uint16_t settings_get_encoder_type() {
  return settings_spanker.get_param(1);
}

void settings_begin() {
  settings_spanker.begin();
}
