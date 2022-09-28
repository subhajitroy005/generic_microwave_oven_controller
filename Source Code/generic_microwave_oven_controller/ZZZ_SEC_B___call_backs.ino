/**********************************************************
 * state name:  state_init_var_reset
 * parent state call: state_init_call
***********************************************************/

void cb_variable_reset(struct application_info* app){
  // Microwave Grill Combination mode variable reset to MIRCROWAVE
  app->op_mode         = MODE_MICROWAVE;
  app->timer.user_timer_setup       = MIN_TIMER_OPERATION_IN_S;               // Reset value of the timer operation
  app->env_condition = CONDITION_DOOR_CLOSED;
  app->ui.notification_flag = NOTIFY_NULL;  // At beginig no update

  // soft start related variables
  soft_start_counter = 0;
  soft_start_activation_flag = 0;
  soft_start_ui_indication = false;
}

/**********************************************************
 * state name:  state_init_io_driver
 * parent state call: state_init_call
***********************************************************/

void cb_io_driver_int(struct application_info* app){
   // input IO pins initialization
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
        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].pressed_state  = BOTH_STATE;
        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].debounce_time  = 30; // Debounce time in 30mS
        scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].previous_state = LOW; // as expecting DOOR is closed by default
        pinMode(scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].pin , INPUT_PULLUP);
         

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

        // Interrupt pin configuration
        scan_pin[INDEX_SCAN_PIN_ZERO_CROSS].pin             = PIN_ZERO_CROSS;
        pinMode(scan_pin[INDEX_SCAN_PIN_ZERO_CROSS].pin , INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(2), zcd_zerocrossing , RISING); // rising mode to capture zero crossing

        
        
        /* OutPut pin Inits*/
        output_pin[INDEX_OUT_PIN_MICROWAVE].pin = PIN_MICROWAVE;
        pinMode(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW);

        output_pin[INDEX_OUT_PIN_LIGHT].pin = PIN_LIGHT;
        pinMode(output_pin[INDEX_OUT_PIN_LIGHT].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , HIGH);

        output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin = PIN_PLATFORM_MOTOR;
        pinMode(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , HIGH);

        output_pin[INDEX_OUT_PIN_FAN].pin = PIN_FAN;
        pinMode(output_pin[INDEX_OUT_PIN_FAN].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , LOW);
        
        /*        
        output_pin[INDEX_OUT_PIN_GRILL].pin = PIN_GRILL;
        pinMode(output_pin[INDEX_OUT_PIN_GRILL].pin , OUTPUT);
        digitalWrite(output_pin[INDEX_OUT_PIN_GRILL].pin , HIGH);
        */
        
}

/**********************************************************
 * state name:  state_init_external
 * parent state call: state_init_call
***********************************************************/

void cb_external_peripheral_init(struct application_info* app){
  /* Timer counter 0 interrupt settings for Soft start application */
  TCCR1A = 0;
  TCCR1B = 0; // timer stopped
  TIMSK1 = 0b00000100; // timer compare vector B 
  /* LCD drivers settings */
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
          if( (scan_pin[i].present_state == scan_pin[i].pressed_state) && (scan_pin[i].pressed_state != BOTH_STATE) ){ // A button press registered but not for both edge detection logic
            scan_pin[i].is_pressed = CONDITION_REGISTERED;
          }
          if(scan_pin[i].pressed_state == BOTH_STATE){
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
          app->cap_signal = SIGNAL_START_OP; // Start signal captured
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_STOP:
          app->cap_signal = SIGNAL_STOP_OP; // Stop signal captured
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_DOOR_OPEN:
          app->cap_signal = SIGNAL_DOOR_OPEN; // Door open signal captured for both open and close state
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
   if((app->timer.user_timer_setup < MIN_TIMER_OPERATION_IN_S) || ( app->timer.user_timer_setup > MAX_TIMER_OPERATION_IN_S)) // max uint_32T rolover in negative condition
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

void cb_door_status_action(struct application_info* app){
  if(scan_pin[INDEX_SCAN_PIN_DOOR_OPEN].present_state == HIGH){ // door opened and notify for door opened
    app->env_condition = CONDITION_DOOR_OPEN;
    app->ui.notification_flag = NOTIFY_DOOR_STATUS;
    // output action
    cb_output_signal_generation(OUT_SIG_DOOR_OPEN);
  }
   else { // for close condition notify nothing
    app->env_condition = CONDITION_DOOR_CLOSED;
    app->ui.notification_flag = NOTIFY_NULL;
    cb_output_signal_generation(OUT_SIG_DOOR_CLOSED);
   }  
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

uint8_t cb_check_start_condition(struct application_info* app){
  // for door open condition notify user other cases just common update
  if(app->env_condition == CONDITION_DOOR_CLOSED){
    return true;
  } else
    app->ui.notification_flag = NOTIFY_DOOR_STATUS; // update for door status

  // other conditoins here 

  // fail safe condition return false always
  return false;
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_ui_update(struct application_info* app){
  lcd.clear(); // clear all the contents

  /* Timer part update */
  uint32_t temp_timer_select;
  if(app->curr_state == STATE_RUNNING)
    temp_timer_select = app->timer.running_timer_status; //update from running timer status
  else
    temp_timer_select = app->timer.user_timer_setup; // update from user timer status
  
  lcd.setCursor(0,0);
  if(temp_timer_select <= 60){    // Update in seconds
    lcd.print(temp_timer_select); 
    lcd.setCursor(util_calculate_digit(temp_timer_select),0);                     // As the above value <60 to unit will be after 2 digit space
    lcd.print("s");              
  } else {                                  // update in minutes
    uint32_t temp_timer_val = (temp_timer_select/60);
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

 /* Notification area update */
  switch(app->ui.notification_flag){
    case NOTIFY_DOOR_STATUS:
           lcd.setCursor(0,1);
           lcd.print("Close the door");
    break;

    case NOTIFY_RUNNING_STATUS:
      // just update running state condition
      lcd.setCursor(0,1);
      lcd.print("Heating...");
    break;

    case NOTIFY_OP_DONE:
      lcd.setCursor(0,1);
      lcd.print("Food is ready");
      app->ui.notification_flag = NOTIFY_NULL;
    break;
  }

  /* Soft start indication */
  if(soft_start_ui_indication == true){
    lcd.setCursor(7,0);
    lcd.print("*");
  }
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/
void cb_output_signal_generation(uint8_t signal_name){
  switch(signal_name){

    case OUT_STARTUP_DEFAULT:
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW); // Micro wave off
      digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , HIGH); // Platform motor off off
      digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , LOW); // Blower fan LOW
      digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , HIGH); //  Light off
    break;
    
    case OUT_SIG_DOOR_OPEN:
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW); // Micro wave off
      digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , HIGH); // Platform motor off off
      digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , LOW); // Blower fan LOW
      digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , LOW); //  Light ON
    break;

    case OUT_SIG_DOOR_CLOSED:
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW); // Micro wave off
      digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , HIGH); // Platform motor off off
      digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , LOW); // Blower fan LOW
      digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , HIGH); //  Light off
    break;

    case OUT_SIG_RUIING:
      digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , LOW); // Platform motor ON
      delay(100);
      digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , HIGH); // Blower fan ON
      delay(100);
      digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , LOW); //  Light ON
      delay(100);
      // activate the soft start
      soft_start_counter = 0;
      soft_start_ui_indication = true;
      soft_start_enable = true;
      
      
    break;

    case OUT_SIGNAL_STOPPED:
      TCCR1B = 0; // stop the timer
      soft_start_activation_flag = false;
      soft_start_enable = false; // disable softstart
      soft_start_counter = 0;
      soft_start_ui_indication = false; // soft start completed
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW); // Micro wave off
      digitalWrite(output_pin[INDEX_OUT_PIN_PLATFORM_MOTOR].pin , HIGH); // Platform motor off off
      digitalWrite(output_pin[INDEX_OUT_PIN_FAN].pin , LOW); // Blower fan LOW
      digitalWrite(output_pin[INDEX_OUT_PIN_LIGHT].pin , HIGH); //  Light off
    break;
  }
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void cb_transition_create_signal(struct application_info* app, uint8_t set_signal){
  app->cap_signal = set_signal;
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
