// ensure this library description is only included once
#ifndef SPANK_fxn_h
#define SPANK_fxn_h

#define LONGEST_PULSE 0
#define SHORTEST_PULSE 1
#define NUM_PULSES 2
#define RANDOMNESS 3
#define INITIAL_DELAY 4

// include types & constants of Wiring core API
#include "Arduino.h"
#include <TerminalVT100.h>
#include <Greenface_EEPROM.h>
#include <EEPROM_Arr16.h>
#include "SPANK_ui.h"

// library interface description
class SPANK_fxn
{
    // user-accessible "public" interface
public:
    typedef void(SPANK_fxn::*FunctionPointer)();
    SPANK_fxn(String _name,String *_labels, uint16_t ** _params, uint16_t _num_params, SPANK_ui *_ui);
    String name;
    String *labels;
    boolean check_params=false;

    uint16_t num_params, param_num;
    uint8_t digit_num;
    void begin(void);
    uint16_t get_param(int16_t indx=-1);
    uint16_t get_min(int16_t indx=-1);
    uint16_t get_max(int16_t indx=-1);
    void adjust_param(int e, unsigned long delta);
    void put_param(uint16_t val);
    int check_param(int the_param);
    void inc_param_num_by(int val);
    void inc_dig_num_by(int val);
    void hilight_param(void);
    String get_label(int label_num);
    void printParams();
    String toJSON(String, String);
    String params_toJSON(); // for wifi

    void display(void); // calls vectored fxn
    void copy_to(SPANK_fxn &target);
    void debug(void);
    uint16_t* mins;
    uint16_t* maxs;

    int offset(void);

    // vectored fxns
    FunctionPointer display_fxn;
    void(*trigger_fxn)();

    // library-accessible "private" interface
private:
    int ui_lines[3];
    SPANK_ui *ui;
    EEPROM_Arr16 params;
    void default_display(void); // prints name and params
    uint8_t get_num_digits(int);
    String calc_format(uint8_t);
};
#endif
