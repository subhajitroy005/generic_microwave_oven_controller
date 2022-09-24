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
  CONDITION_DOOR_CLOSED,
};

enum notification_update_code{
  NOTIFY_DOOR_STATUS = 0 ,
  NOTIFY_RUNNING_STATUS,
  NOTIFY_OP_DONE,
  NOTIFY_NULL
};

enum output_io_signal_list{
  OUT_STARTUP_DEFAULT,
  OUT_SIG_DOOR_OPEN,
  OUT_SIG_DOOR_CLOSED,
  OUT_SIG_RUIING,
  OUT_SIGNAL_STOPPED
};
/* GLOBAL Objects and variables*/
LiquidCrystal_I2C lcd(0x27,16,2);     // set the LCD address to 0x27 for a 16 chars and 2 line display


typedef struct _timer_information
        {
            uint32_t       user_timer_setup;  // Running time set by user
            uint64_t       operation_start_timestamp;
            uint32_t       running_timer_status;
            uint32_t       ui_refresh_ts; // for UI refresh purpose    
            uint32_t       minute_display_scaling_factor_hold;
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
  SIGNAL_START_OP,
  SIGNAL_STOP_OP,
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
#define MAX_OUT_PINS              4             // Maximum output pins 

// OUTPUT PINS digital IO number-----------
#define PIN_MICROWAVE             10 //  PB2 -> Pin 16
//#define PIN_GRILL                 9 
#define PIN_LIGHT                 11 // PB3 -> Pin 17
#define PIN_PLATFORM_MOTOR        12 // PB4 -> Pin 18 
#define PIN_FAN                   8 // PB0 -> Pin 14

//INPUT PINS digital IO number-----------
#define PIN_START                 9 // PB1 - > Pin 15
#define PIN_STOP                  3 // PD3 -> Pin 5
#define PIN_TIMER_UP              4 // PD4 -> Pin 6
#define PIN_TIMER_DOWN            5 // PD5 -> Pin 11
#define PIN_MODE_SELECT           6 // PD6 -> Pin 12
#define PIN_DOOR_OPEN             7 // PD7 -> Pin 13
#define PIN_ZERO_CROSS            2 // PD2 -> Pin 4 // interrupt // not in polling

/* LOGIG States */
#define DOOR_OPEN_STATE           1
#define DOOR_CLOSED_STATE         0

enum  scan_pin_index_poll{                   // SCAN pin indexes for polling
  INDEX_SCAN_PIN_START = 0,
  INDEX_SCAN_PIN_STOP,
  INDEX_SCAN_PIN_DOOR_OPEN,
  INDEX_SCAN_PIN_TIMER_DOWN,
  INDEX_SCPN_PIN_MODE_SELECT,
  INDEX_SCAN_PIN_TIMER_UP,
  INDEX_SCAN_PIN_ZERO_CROSS  
};
enum out_pin_index{                         // Output pin indexes for driver purpose
  INDEX_OUT_PIN_MICROWAVE = 0,
  //INDEX_OUT_PIN_GRILL,
  INDEX_OUT_PIN_LIGHT,
  INDEX_OUT_PIN_PLATFORM_MOTOR,
  INDEX_OUT_PIN_FAN
};

volatile struct _scan_pins
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
        
volatile struct _output_pins
        {
            uint8_t        pin;
            uint32_t       pin_state;
            
        }output_pin[MAX_OUT_PINS];
//********************************************//
//********************************************//

volatile uint8_t soft_start_enable = false;
volatile int soft_start_counter = 0;
volatile uint8_t soft_start_activation_flag = 0;
volatile uint8_t soft_start_ui_indication = 0;
