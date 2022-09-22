/**********************************************************
 * state name:  state_init_var_reset
 * parent state call: state_init_call
***********************************************************/

void cb_variable_reset(struct application_info* app){
  // Microwave Grill Combination mode variable reset to MIRCROWAVE
  app->op_mode         = MODE_MICROWAVE;
  app->timer.user_timer_setup       = MIN_TIMER_OPERATION_IN_S;               // Reset value of the timer operation
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

void cb_io_driver_int(struct application_info* app){
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

void cb_external_peripheral_init(struct application_info* app){
  lcd.init();                     // Initialize the LCD
  lcd.backlight();                // On the LCD Back Light
}

/**********************************************************
 * state name:  state_init_call
 * parent state call: MAIN
***********************************************************/
void cb_signal_polling(struct application_info* app){
  for(int i = 0 ; i < MAX_IO_SCAN_PINS ; i++){
    scan_pin[i].pin_state = digitalRead(scan_pin[i].pin); // Scan the pin
    
    if (scan_pin[i].pin_state != scan_pin[i].previous_state) {
      scan_pin[i].last_debounce_time = millis();
    }
    
    if ((millis() - scan_pin[i].last_debounce_time) >= scan_pin[i].debounce_time){  // > because the micros are not precise eough
      if (scan_pin[i].pin_state != scan_pin[i].present_state){
          scan_pin[i].present_state = scan_pin[i].pin_state;
          if(scan_pin[i].present_state == scan_pin[i].pressed_state ){ // A button press registered
            scan_pin[i].is_pressed = CONDITION_REGISTERED;
           }
      }
    }
    scan_pin[i].previous_state = scan_pin[i].pin_state;
  }

  // based on the physical signal  state machine sw signlas will be generated
  cb_state_machine_signal_gen(app);
}


/**********************************************************
 * state name:  state_signal_action_dispenser_call
 * parent state call: MAIN
***********************************************************/

void cb_state_machine_signal_gen(struct application_info* app){
  for(int i = 0 ; i < MAX_IO_SCAN_PINS ; i++){
    if(scan_pin[i].is_pressed == CONDITION_REGISTERED){
       scan_pin[i].is_pressed = CONDITION_SERVED; // one time signal condition served

       switch(i){ // dispense according to signal index

        //-------------------------------------------------
        case INDEX_SCAN_PIN_TIMER_UP:
          app->cap_signal = SIGNAL_TIMER_UP; // timer up signal captured
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_TIMER_DOWN:
          app->cap_signal = SIGNAL_TIMER_DOWN; // timer up signal captured     
        break;

        //-------------------------------------------------
        case INDEX_SCPN_PIN_MODE_SELECT:
          app->cap_signal = SIGNAL_MODE_SELECT; // Mode select signal captured
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_START:
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_STOP:
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_DOOR_OPEN:
        break;
        
       }
    }
  }
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

uint8_t cb_self_test(struct application_info* app){

  /* test the peripherals and update the transition */
  // As of now no test case to return true

  return true;
}


/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_timer_up_action(struct application_info* app){
  if(app->timer.user_timer_setup <60){ // within 60s range
    app->timer.user_timer_setup += 15; // Add 15s
  } else if( (app->timer.user_timer_setup >= 60) && (app->timer.user_timer_setup < 1200) ){ // within 1m to 20m range
    app->timer.user_timer_setup += 60; // Add 1m [60 s]
  } else {
    app->timer.user_timer_setup += 600; // Add 10m [600 s] 
  }
  /* Maximum Guard condition of the timer*/
  if(app->timer.user_timer_setup > MAX_TIMER_OPERATION_IN_S)
    app->timer.user_timer_setup = MAX_TIMER_OPERATION_IN_S;
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_timer_down_action(struct application_info* app){
   if(app->timer.user_timer_setup > 1200){
     app->timer.user_timer_setup -= 600; // Substract 10m [600s]
   } else if( (app->timer.user_timer_setup > 60) && (app->timer.user_timer_setup <= 1200) ){
     app->timer.user_timer_setup -= 60; // Substract 1m [60s]
   } else {
     app->timer.user_timer_setup -= 15; // Substract 15s
   }
   /* Minimum Guard condition of the timer*/
   if(app->timer.user_timer_setup < MIN_TIMER_OPERATION_IN_S)
      app->timer.user_timer_setup = MIN_TIMER_OPERATION_IN_S;
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_mode_select_action(struct application_info* app){
  app->op_mode++; // update the operation mode
  
  // Rollover condition of mode
  if(app->op_mode >= MAX_MODE)
    app->op_mode = 0; // Set the first mode
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_ui_update(struct application_info* app){
  lcd.clear(); // clear all the contents

  /* Timer part update */
  lcd.setCursor(0,0);
  if(app->timer.user_timer_setup <= 60){    // Update in seconds
    lcd.print(app->timer.user_timer_setup); 
    lcd.setCursor(2,0);                     // As the above value <60 to unit will be after 2 digit space
    lcd.print("s");              
  } else {                                  // update in minutes
    uint32_t temp_timer_val = (app->timer.user_timer_setup/60);
    lcd.print(temp_timer_val); // Update the time value in dispaly (floor op)
    lcd.setCursor(util_calculate_digit(temp_timer_val),0); // calculate the number of digit and set the cursor there as lne start form 0
    lcd.print("m");
  }

  /* Mode part update */
  lcd.setCursor(10,0);
  switch(app->op_mode){
    case MODE_MICROWAVE:
      lcd.print("Micro");
    break;

    case MODE_GRILL:
      lcd.print("Grill");
    break;
  }
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_transition_entry(struct application_info* app){
  
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_transition_destroy_signal(struct application_info* app){
  app->cap_signal = SIGNAL_NULL;
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_transition_exit(struct application_info* app , uint8_t next_state){
  app->curr_state = next_state; // assign the state foe next transition
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

uint8_t util_calculate_digit(uint32_t digit){  
   int count=0;
   while(digit!=0){  
    digit=digit/10;  
    count++;  
   }
   return count;
}