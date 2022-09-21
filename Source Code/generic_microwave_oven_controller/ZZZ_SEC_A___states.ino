/**********************************************************
 * state name:  state_init_call
 * parent state call: MAIN
***********************************************************/

void state_init_call(){

  // All system variable reset
  state_init_var_reset();
  
  // IO driver init
  state_init_io_driver();

  // External devices initialization
  state_init_external();

  // global update UI request after all initialization
  global_ui_update_request = CONDITION_REGISTERED;
   
}

/**********************************************************
 * state name:  state_init_call
 * parent state call: MAIN
***********************************************************/
void state_io_polling_call(){
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
}



/**********************************************************
 * state name:  state_signal_action_dispenser_call
 * parent state call: MAIN
***********************************************************/

void state_signal_action_dispenser_call(){
  for(int i = 0 ; i < MAX_IO_SCAN_PINS ; i++){
    if(scan_pin[i].is_pressed == CONDITION_REGISTERED){
       scan_pin[i].is_pressed = CONDITION_SERVED; // one time signal condition served

       switch(i){ // dispense according to signal index

        //-------------------------------------------------
        case INDEX_SCAN_PIN_TIMER_UP:
        
          if(timer_operation <60){ // within 60s range
            timer_operation += 15; // Add 15s
          } else if( (timer_operation >= 60) && (timer_operation < 1200) ){ // within 1m to 20m range
            timer_operation += 60; // Add 1m [60 s]
          } else {
            timer_operation += 600; // Add 10m [600 s] 
          }
          /* Maximum Guard condition of the timer*/
          if(timer_operation > MAX_TIMER_OPERATION_IN_S)
            timer_operation = MAX_TIMER_OPERATION_IN_S;
            
        break;

        //-------------------------------------------------
        case INDEX_SCAN_PIN_TIMER_DOWN:

          if(timer_operation > 1200){
            timer_operation -= 600; // Substract 10m [600s]
          } else if( (timer_operation > 60) && (timer_operation <= 1200) ){
            timer_operation -= 60; // Substract 1m [60s]
          } else {
             timer_operation -= 15; // Substract 15s
          }
          /* Minimum Guard condition of the timer*/
          if(timer_operation < MIN_TIMER_OPERATION_IN_S)
             timer_operation = MIN_TIMER_OPERATION_IN_S;
             
        break;

        //-------------------------------------------------
        case INDEX_SCPN_PIN_MODE_SELECT:
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

       // After any signal change just update the UI
       global_ui_update_request = CONDITION_REGISTERED;
    }
  }
}

/**********************************************************
 * state name:  state_signal_action_dispenser_call
 * parent state call: MAIN
***********************************************************/
void state_back_op_call(){



  // UI update
  if(global_ui_update_request == CONDITION_REGISTERED){
    global_ui_update_request = CONDITION_SERVED;
    state_back_op_ui_update_call();    
  }
}
