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
      }
    break;

    /*
     * State name: STATE_IO_POLLING
     * Brief info:
     * ---------------------------------------------------------------
    */
    case STATE_RUNNING:
      switch (application.cap_signal){
        
      }
      cb_transition_exit(&application , STATE_SELF_TEST);
    break;
    

    
  }
}
