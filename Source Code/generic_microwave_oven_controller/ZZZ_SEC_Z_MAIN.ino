void setup(){
  // Init state call
  state_init_call();

  sys_state.state_tran_ind = STATE_IO_POLLING; // next state
}

void loop() {

  // core state machine
  switch(sys_state.state_tran_ind){

    /*
     * State name: STATE_IO_POLLING
     * Brief info:
    */
    case STATE_IO_POLLING:
      state_io_polling_call();
      sys_state.state_tran_ind = STATE_SIGNAL_ACTION; // next state
    break;

    /*
     * State name: STATE_SIGNAL_ACTION
     * Brief info:
    */
    case STATE_SIGNAL_ACTION:
      state_signal_action_dispenser_call();
      sys_state.state_tran_ind = STATE_BACKGROUND_OP; // next state
    break;

    /*
     * State name: STATE_IO_POLLING
     * Brief info:
    */
    case STATE_BACKGROUND_OP:
      state_back_op_call();
      sys_state.state_tran_ind = STATE_IO_POLLING; // next state
    break;
    
    
  }
    

}
