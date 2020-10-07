/*
  RotaryEncoder - RotaryEncoder library 
  Copyright (c) 2019 Shannon Vance.  All right reserved.
  Released under Creative Commons Attribution-ShareAlike 2.5 Generic License
*/

// ensure this library description is only included once
#ifndef RotaryEncoder_h
#define RotaryEncoder_h

// states
#define REST 0
#define CW 1
#define CCW 2
#define UNK 3

// include types & constants of Wiring core API


// library interface description
class RotaryEncoder
{
  // user-accessible "public" interface
  public:
    RotaryEncoder();
	int numFxns;
    void incEncoder(void);
    void decEncoder(void);
	int getEncoderValue(void);
	void setEncoderValue(int);
	void updateEncoder(void);
	byte msb_pin=2;
	byte lsb_pin=3;
	void aChanInt(void);
	void bChanInt(void);
	// TerminalVT100 t;
	bool debug = false;
	
  // library-accessible "private" interface
  private:
	int state;
	int encoderValue;
	String channel; // A or B
};

extern RotaryEncoder e;

#endif

