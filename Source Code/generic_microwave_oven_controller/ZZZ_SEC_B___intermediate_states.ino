/**********************************************************
 * state name:  state_init_var_reset
 * parent state call: state_init_call
***********************************************************/

void state_init_var_reset(){
  // Microwave Grill Combination mode variable reset to MIRCROWAVE
  mode_selector         = MODE_MICROWAVE;
  timer_operation       = MIN_TIMER_OPERATION_IN_S;               // Reset value of the timer operation
  machine_status        = STATUS_OK;        // Default Status at reset

  // Timer operation related variable
  lcd_time_counter      = 0; // Starting counter zero
  time_tracker          = 0;
  mark_time             = 0;

  
}

/**********************************************************
 * state name:  state_init_io_driver
 * parent state call: state_init_call
***********************************************************/

void state_init_io_driver(){
   // input IO pins initialization
   for(int i = 0; i<MAX_IO_SCAN_PINS; i++){
    
   }
        scan_pin[INDEX_SCAN_PIN_START].pin            = PIN_START;
        scan_pin[INDEX_SCAN_PIN_START].pressed_state  = LOW;
        scan_pin[INDEX_SCAN_PIN_START].debounce_time  = 30; // Debounce time in 30mS
        scan_pin[INDEX_SCAN_PIN_START].previous_state = (!scan_pin[INDEX_SCAN_PIN_START].pressed_state); // default state as system statup expectation not pressed condition
        pinMode(scan_pin[INDEX_SCAN_PIN_START].pin , INPUT_PULLUP);
        

        scan_pin[INDEX_SCAN_PIN_STOP].pin             = PIN_STOP;
        scan_pin[INDEX_SCAN_PIN_STOP].pressed_state   = LOW;
        scan_pin[INDEX_SCAN_PIN_STOP].debounce_time   = 30; // Debounce time in 30mS
        scan_pin[INDEX_SCAN_PIN_STOP].previous_state  = (!scan_pin[INDEX_SCAN_PIN_STOP].pressed_state);
        pinMode(scan_pin[INDEX_SCAN_PIN_STOP].pin , INPUT_PULLUP);
        

        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].pin            = PIN_DOOR_OPEN;
        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].pressed_state  = HIGH;
        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].debounce_time  = 30; // Debounce time in 30mS
        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].previous_state = (!scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].pressed_state);
        pinMode(scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].pin , INPUT);
         

        scan_pin[INDEX_SCAN_PIN_TIMER_DOWN].pin             = PIN_TIMER_DOWN;
        scan_pin[INDEX_SCAN_PIN_TIMER_DOWN].pressed_state   = LOW;
        scan_pin[INDEX_SCAN_PIN_TIMER_DOWN].debounce_time   = 30;
        scan_pin[INDEX_SCAN_PIN_TIMER_DOWN].previous_state  = (!scan_pin[INDEX_SCAN_PIN_TIMER_DOWN].pressed_state); 
        pinMode(scan_pin[INDEX_SCAN_PIN_TIMER_DOWN].pin , INPUT_PULLUP);
        

        scan_pin[INDEX_SCPN_PIN_MODE_SELECT].pin            = PIN_MODE_SELECT;
        scan_pin[INDEX_SCPN_PIN_MODE_SELECT].pressed_state  = LOW;
        scan_pin[INDEX_SCPN_PIN_MODE_SELECT].debounce_time  = 30;
        scan_pin[INDEX_SCPN_PIN_MODE_SELECT].previous_state = (!scan_pin[INDEX_SCPN_PIN_MODE_SELECT].pressed_state); 
        pinMode(scan_pin[INDEX_SCPN_PIN_MODE_SELECT].pin , INPUT_PULLUP);
        

        scan_pin[INDEX_SCAN_PIN_TIMER_UP].pin             = PIN_TIMER_UP;
        scan_pin[INDEX_SCAN_PIN_TIMER_UP].pressed_state   = LOW;
        scan_pin[INDEX_SCAN_PIN_TIMER_UP].debounce_time   = 30; // Debounce time in 30mS
        scan_pin[INDEX_SCAN_PIN_TIMER_UP].previous_state  = (!scan_pin[INDEX_SCAN_PIN_TIMER_UP].pressed_state);
        pinMode(scan_pin[INDEX_SCAN_PIN_TIMER_UP].pin , INPUT_PULLUP);
        
        

        /* OutPut pin Inits*/
        output_pin[INDEX_OUT_PIN_MICROWAVE].pin = PIN_MICROWAVE;
        pinMode(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , HIGH);

        output_pin[INDEX_OUT_PIN_GRILL].pin = PIN_GRILL;
        pinMode(output_pin[INDEX_OUT_PIN_GRILL].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_GRILL].pin , HIGH);

        output_pin[INDEX_OUT_PIN_LIGHT].pin = PIN_LIGHT;
        pinMode(output_pin[INDEX_OUT_PIN_LIGHT].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , HIGH);

        output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin = PIN_PLATFORM_MOTOR;
        pinMode(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , HIGH);

        output_pin[INDEX_OUT_PIN_FAN].pin = PIN_FAN;
        pinMode(output_pin[INDEX_OUT_PIN_FAN].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , LOW);

        
}

/**********************************************************
 * state name:  state_init_external
 * parent state call: state_init_call
***********************************************************/

void state_init_external(){
  lcd.init();                     // Initialize the LCD
  lcd.backlight();                // On the LCD Back Light
}

/**********************************************************
 * state name:  state_back_op_ui_update_call
 * parent state call: state_back_op_call
***********************************************************/

void state_back_op_ui_update_call(){
  lcd.clear(); // clear all the contents
  /* Fixed part of UI updates */
  lcd.setCursor(0,0);
  lcd.print("T");
  lcd.setCursor(1,0);
  lcd.print("-");
  lcd.setCursor(13,0);
  lcd.print("M");
  lcd.setCursor(14,0);
  lcd.print("G");
  lcd.setCursor(15,0);
  lcd.print("C");

  /*Microwave Grill Combination Selection*/
        switch(mode_selector)
            {
                case MODE_MICROWAVE:
                    lcd.setCursor(13,1);
                    lcd.print("^");
                break;

                case MODE_GRILL:
                    lcd.setCursor(14,1);
                    lcd.print("^");
                break;

                case MODE_COMBINATION:
                    lcd.setCursor(15,1);
                    lcd.print("^");
                break;

                default:
                break;   
            }

        /* Timer value update */
      if(timer_operation <= 60)
          {
              lcd.setCursor(2,0);
              lcd.print(timer_operation);
              lcd.setCursor(5,0);
              lcd.print("S");              
          }
      else
          {
              lcd.setCursor(2,0);
              lcd.print((timer_operation/60));
              lcd.setCursor(5,0);
              lcd.print("M");
          }
      /* Print status of the machine*/

       switch(machine_status)
          {
              case STATUS_DOOR_OPEN: 
                  lcd.setCursor(0,1);
                  lcd.print("DOOR OPEN");
              break;

              case STATUS_DOOR_CLOSE: 
                  lcd.setCursor(0,1);
                  lcd.print("READY");
              break;

              case STATUS_RUNNING:
                  lcd.setCursor(0,1);
                  lcd.print("RUN");

                  
              break;

              case STATUS_STOP:
                  lcd.setCursor(0,1);
                  lcd.print("STOP");
              break;
              
              case STATUS_OK:
                  lcd.setCursor(0,1);
                  lcd.print("OK");
              break;

              case STATUS_ERR:
                  lcd.setCursor(0,1);
                  lcd.print("ERR !!!");
              break;

              default:
              break;   
          }
 
      /* Timer operation update */

      lcd.setCursor(7,0);
      lcd.print(">");
      lcd.setCursor(8,0);
      lcd.print(lcd_time_counter);
}
