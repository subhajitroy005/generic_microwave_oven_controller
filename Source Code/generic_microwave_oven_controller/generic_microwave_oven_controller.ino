/*
* Generic microwave oven controller.
* subhajitroy005@gmail.com
* TAB size 2
*/

/* Architecture specific macros */
#define ELECTROLUX_C23K101

// ----  Common include
#include <stdint.h>

// Devive specific include
#ifdef ELECTROLUX_C23K101
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
#endif 

#define BOTH_STATE           2
#define CONDITION_REGISTERED true
#define CONDITION_SERVED     false




/* Syatem Constrains */
#define MAX_TIMER_OPERATION_IN_S  18000
#define MIN_TIMER_OPERATION_IN_S  15






/* Operation constrains */
#define MAX_MODE  2
enum _microwave_modes{
  MODE_MICROWAVE,
  MODE_GRILL
};

enum enum_env_condition{
  CONDITION_DOOR_OPEN,
  CONDITION_DOOR_CLOSED
};

enum notification_update_code{
  NOTIFY_DOOR_STATUS = 0 ,
  NOTIFY_RUNNING_STATUS,
  NOTIFY_NULL
};

/* GLOBAL Objects and variables*/
LiquidCrystal_I2C lcd(0x27,16,2);     // set the LCD address to 0x27 for a 16 chars and 2 line display


typedef struct _timer_information
        {
            uint32_t       user_timer_setup;  // Running time set by user
            
        }timer_info;

typedef struct _ui_information
        {
            uint8_t       notification_flag;  // Running time set by user
            
        }ui_info;

// State related structures and enums -------------------------------------------------------
//-------------------------------------------------------------------------------------------


typedef enum state_list{
  //STATE_INIT  // Not listed because this is static init and implemented in setup function 
  STATE_SELF_TEST = 0 , 
  STATE_IDLE,
  STATE_RUNNING
};
enum signals_list{
  SIGNAL_SELF_TEST_OK = 0,
  SIGNAL_SELF_TEST_FAIL,
  SIGNAL_TIMER_UP,
  SIGNAL_TIMER_DOWN,
  SIGNAL_MODE_SELECT,
  SIGNAL_DOOR_OPEN,
  SIGNAL_NULL 
};

struct application_info
        {
            uint8_t        next_state;  // Hold the state for next state transition
            uint8_t        curr_state;  // Hold the state from where the next state is transiting
            uint8_t        cap_signal;  // captured signal

            
            uint8_t        op_mode;     // Operation modes select by user

            uint8_t        env_condition;

            
            // ---  Timer related info
            timer_info     timer;       // timer

            // --- UI Info
            ui_info        ui;
        }application;



//********************************************//
//********************************************//
// IO related section //
//********************************************//
//********************************************//
/* State related variables */
#define MAX_IO_SCAN_PINS          6             // maximum scan pins for user input
#define MAX_OUT_PINS              5             // Maximum output pins 

// OUTPUT PINS digital IO number-----------
#define PIN_MICROWAVE             8
#define PIN_GRILL                 9
#define PIN_LIGHT                 10
#define PIN_PLATFORM_MOTOR        11
#define PIN_FAN                   12

//INPUT PINS digital IO number-----------
#define PIN_START                 2
#define PIN_STOP                  3
#define PIN_TIMER_UP              4
#define PIN_TIMER_DOWN            5
#define PIN_MODE_SELECT           6
#define PIN_DOOR_OPEN             7
/* LOGIG States */
#define DOOR_OPEN_STATE           1
#define DOOR_CLOSED_STATE         0

enum  scan_pin_index_poll{                   // SCAN pin indexes for polling
  INDEX_SCAN_PIN_START = 0,
  INDEX_SCAN_PIN_STOP,
  INDEX_SCAN_PIN_DOOR_OPEN,
  INDEX_SCAN_PIN_TIMER_DOWN,
  INDEX_SCPN_PIN_MODE_SELECT,
  INDEX_SCAN_PIN_TIMER_UP  
};
enum out_pin_index{                         // Output pin indexes for driver purpose
  INDEX_OUT_PIN_MICROWAVE = 0,
  INDEX_OUT_PIN_GRILL,
  INDEX_OUT_PIN_LIGHT,
  INDEX_OUT_PIN_PLATFORM_MOTOR,
  INDEX_OUT_PIN_FAN
};

struct _scan_pins
        {
            uint8_t        pin;
            uint8_t        pin_state;            // Current state at polling
            uint8_t        previous_state;       // Debounce purpose comparison purpose
            uint8_t        present_state;        // Reflect tyhe hold state
            uint32_t       debounce_time;
            uint32_t       last_debounce_time;   // State change debounce time capture
            uint8_t        pressed_state;        // Configuration based on circuit design high/low
            uint8_t        is_pressed;           // Flag for resiater a signa for button is pressed
        }scan_pin[MAX_IO_SCAN_PINS];
        
struct _output_pins
        {
            uint8_t        pin;
            uint32_t       pin_state;
            
        }output_pin[MAX_OUT_PINS];
//********************************************//
//********************************************//




unsigned long time_tracker;           // Tracking the time for operation
unsigned long mark_time;              // Mark the time from where the operation should start
unsigned long lcd_update_time;        // store the last time for LCD update
unsigned long lcd_time_counter;
unsigned char operation_flag;         // Start/Stop condition of the operation
