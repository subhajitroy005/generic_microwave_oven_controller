void setup(){
  // Init state call
  state_init_call(&application);
  
  cb_transition_exit(&application , STATE_SELF_TEST);
}

void loop() {
  // Signal polling
  cb_signal_polling(&application);
  
  // State machine core
  switch(application.curr_state){

    /*
     * State name: STATE_IO_POLLING
     * Brief info:
     * ---------------------------------------------------------------
    */
    case STATE_SELF_TEST:
      if(cb_self_test(&application)){
        cb_transition_exit(&application , STATE_IDLE); // if test result is OK then idle otherwise in tis state
      }
    break;

    /*
     * State name: STATE_IO_POLLING
     * Brief info:
     * ---------------------------------------------------------------
    */
    case STATE_IDLE:
      switch (application.cap_signal){
        
        case SIGNAL_TIMER_UP: // timer up button pressed
          cb_timer_up_action(&application);
          cb_ui_update(&application); // update the UI
          cb_transition_destroy_signal(&application); // destroy the signal so that same state it shuld not enter this signal condition
        break;

        case SIGNAL_TIMER_DOWN: // timer down button pressed
          cb_timer_down_action(&application);
          cb_ui_update(&application); // update the UI
          cb_transition_destroy_signal(&application); // destroy the signal so that same state it shuld not enter this signal condition
        break;

        case SIGNAL_MODE_SELECT:
          cb_mode_select_action(&application);
          cb_ui_update(&application); // update the UI
          cb_transition_destroy_signal(&application); // destroy the signal so that same state it shuld not enter this signal condition
        break;

        case SIGNAL_DOOR_OPEN:
           cb_door_status_action(&application);
           cb_ui_update(&application); // update the UI
           cb_transition_destroy_signal(&application); // destroy the signal so that same state it shuld not enter this signal condition
        break;

        case SIGNAL_START_OP:
           if(cb_check_start_condition(&application)){ // if valid start condition the start
            function_operation_start_action();
            application.ui.notification_flag = NOTIFY_RUNNING_STATUS; // update notification
            cb_ui_update(&application); // update the UI
            cb_transition_exit(&application , STATE_RUNNING); // don't destroy the signal as we need loop condition in running state
           } else {
            cb_transition_destroy_signal(&application); // destroy the signal so that same state it shuld not enter this signal condition
           }
        break;
      }
    break;

    /*
     * State name: STATE_IO_POLLING
     * Brief info:
     * ---------------------------------------------------------------
    */
    case STATE_RUNNING:
    
      switch (application.cap_signal){

        case SIGNAL_START_OP: // this signagnal will never destry and looped here
          /* time checking conditoin */
          if( (millis() - application.timer.operation_start_timestamp) > (application.timer.user_timer_setup * 1000) ){ // final timer exit condition // S to mS conversion
              function_operation_stop_action();  
          } else {                                                                            // other case just update UI for user                                                                         
              if((millis() - application.timer.ui_refresh_ts) > 1000){  // update the time to user
                // deduct 1S from the timer counter for diaplay
                application.timer.running_timer_status--; // deduct a second
                if(application.timer.running_timer_status < 60){ // for > a munute update at a second interval
                  cb_ui_update(&application); // update the UI
                } else if((application.timer.minute_display_scaling_factor_hold - application.timer.running_timer_status) > 60 ){ // update a minute interval
                  application.timer.minute_display_scaling_factor_hold = application.timer.running_timer_status; // for next minute update
                  cb_ui_update(&application); // update the UI
                }
                // update the UI refresh TS
                application.timer.ui_refresh_ts = millis(); 
              }   
          }
        break;

        case SIGNAL_MODE_SELECT:
          // nothing to do with mode select so destry the signal and set the signal to running
          cb_transition_destroy_signal(&application);
          cb_transition_create_signal(&application, SIGNAL_START_OP);
        break;
        
        case SIGNAL_STOP_OP:
          function_operation_stop_action();
        break;

        case SIGNAL_DOOR_OPEN:
          function_operation_stop_action();
          cb_door_status_action(&application);
          cb_ui_update(&application); // update the UI
        break;
        
        /* Only timer operation are aloowed here*/
        case SIGNAL_TIMER_UP: // timer up button pressed
          application.timer.user_timer_setup = application.timer.running_timer_status; // first hold the current running status
          cb_timer_up_action(&application);
          // update runtime timer status
          application.timer.running_timer_status = application.timer.user_timer_setup; // this will for visualize the timer for user
          application.timer.minute_display_scaling_factor_hold  = application.timer.running_timer_status;
            
          cb_ui_update(&application); // update the UI
          // go back to signal start op running state
          cb_transition_create_signal(&application, SIGNAL_START_OP);
          cb_transition_exit(&application , STATE_RUNNING);

          // again take the timestamp from point of button activity 
          application.timer.operation_start_timestamp = millis();
        break;

        case SIGNAL_TIMER_DOWN: // timer down button pressed
          application.timer.user_timer_setup = application.timer.running_timer_status; // first hold the current running status
          cb_timer_down_action(&application);
          // update runtime timer status
          application.timer.running_timer_status = application.timer.user_timer_setup; // this will for visualize the timer for user
          application.timer.minute_display_scaling_factor_hold  = application.timer.running_timer_status;
          
          cb_ui_update(&application); // update the UI
          // go back to signal start op running state
          cb_transition_create_signal(&application, SIGNAL_START_OP);
          cb_transition_exit(&application , STATE_RUNNING);

          // again take the timestamp from point of button activity 
          application.timer.operation_start_timestamp = millis();
        break;
      }
    break;
       
  }
}

void function_operation_start_action(){
  // Form this point time will be marked for completion
  application.timer.operation_start_timestamp = millis();
  application.timer.ui_refresh_ts = application.timer.operation_start_timestamp;
  
  application.timer.running_timer_status = application.timer.user_timer_setup; // this will for visualize the timer for user
  application.timer.minute_display_scaling_factor_hold  = application.timer.running_timer_status;

  
}

void function_operation_stop_action(){
  // Transit to idle condition and destroy the signal for safety
  cb_transition_exit(&application , STATE_IDLE);
  cb_transition_destroy_signal(&application);
  
              
  // default time set to minimal user define time
  application.timer.user_timer_setup = MIN_TIMER_OPERATION_IN_S;
  // update UI and notify user
  application.ui.notification_flag = NOTIFY_OP_DONE;
  cb_ui_update(&application); // update the UI
}
