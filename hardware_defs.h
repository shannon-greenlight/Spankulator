// hardware
const byte toggle_out_pin = 5;
const byte repeat_led_pin = 6;
const byte triggered_led_pin = 12;
const byte gate_out_pin = 9;
const byte aout_pin = A0;
const byte ain1_pin = A1;
const byte ain2_pin = A2;
const byte ain3_pin = A3;

// buttons
const byte up_button_pin = 0;
const byte dn_button_pin = 1;
const byte repeat_button_pin = 4;
const byte trigger_button_pin = 7;
const byte disable_trigger_pin = OLD_STYLE ? A3 : 11;
const byte param_up_button_pin = A6;  //8;
const byte param_dn_button_pin = A7;  //10;
const byte left_button_pin = 10; //A6;
const byte right_button_pin = 8; //A7;

// Rotary Encoder
void intFxnA(void) {
  e.aChanInt();
}

void intFxnB(void) {
  e.bChanInt();
}

void hardware_begin() {
  e.numFxns = 0;
  //e.t = ui.t;
  //e.debug = true;

  pinMode(toggle_out_pin, OUTPUT);
  pinMode(gate_out_pin, OUTPUT);
  pinMode(repeat_led_pin, OUTPUT);
  pinMode(disable_trigger_pin, OUTPUT);
  pinMode(triggered_led_pin, OUTPUT);
  pinMode(0, INPUT);
  pinMode(1, INPUT);

  pinMode(up_button_pin, INPUT_PULLUP);
  pinMode(dn_button_pin, INPUT_PULLUP);
  pinMode(repeat_button_pin, INPUT_PULLUP);
  pinMode(trigger_button_pin, INPUT_PULLUP);
  pinMode(param_up_button_pin, INPUT_PULLUP);
  pinMode(param_dn_button_pin, INPUT_PULLUP);
  pinMode(right_button_pin, INPUT_PULLUP);
  pinMode(left_button_pin, INPUT_PULLUP);

  analogWriteResolution(10);
  analogWrite(A0, 511);

}


