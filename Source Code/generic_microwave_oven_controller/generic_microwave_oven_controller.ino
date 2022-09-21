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


#define CONDITION_REGISTERED true
#define CONDITION_SERVED     false




/* Syatem Constrains */
#define MAX_TIMER_OPERATION_IN_S  18000
#define MIN_TIMER_OPERATION_IN_S  15



/* GLOBAL Objects and variables*/

LiquidCrystal_I2C lcd(0x27,16,2);     // set the LCD address to 0x27 for a 16 chars and 2 line display


// State related structures and enums
enum state_tran_list{
  STATE_INIT,
  STATE_IO_POLLING,
  STATE_SIGNAL_ACTION,
  STATE_BACKGROUND_OP
};

struct _state_info
        {
            uint8_t        state_tran_ind;
            
        }sys_state;

enum _microwave_modes{
  MODE_MICROWAVE,
  MODE_GRILL,
  MODE_COMBINATION
};

enum _syatem_status{
  STATUS_DOOR_OPEN,
  STATUS_DOOR_CLOSE,
  STATUS_RUNNING,
  STATUS_STOP,
  STATUS_OK,
  STATUS_ERR
};






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

uint8_t       global_ui_update_request;

unsigned char mode_selector;          // Mode variable for operation

long          timer_operation;        // Store the time for operation
unsigned long time_tracker;           // Tracking the time for operation
unsigned long mark_time;              // Mark the time from where the operation should start
unsigned long lcd_update_time;        // store the last time for LCD update
unsigned long lcd_time_counter;
unsigned char machine_status;         // status of the machine
unsigned char operation_flag;         // Start/Stop condition of the operation

/* function definitions */
void state_init_var_reset();
