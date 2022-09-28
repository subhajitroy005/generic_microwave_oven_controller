/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void zcd_zerocrossing(){
  if(soft_start_enable == true){
    digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW); // Signal OFF at zero crossing point .. sllightly enable at end part
    TCCR1B = 0; // stop the timer
    OCR1B = 250 - soft_start_counter; // 133 as of now hardcoded but calculated from firing angle ND RAMP OF 1 S OF 50 hz SIGNAL
    TCNT1 = 0; // reset the counter value
    soft_start_activation_flag = true; // after time occured make the signal high
    soft_start_counter++;
    soft_start_activation_flag = true;
    // stop prevention
    if(soft_start_counter == 249){ // soft start complte and continious mode to op
      TCCR1B = 0; // stop the timer
      soft_start_activation_flag = false;
      soft_start_enable = false; // disable softstart
      soft_start_counter = 0;
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , HIGH); // continious signal on line
      soft_start_ui_indication = false; // soft start completed
    }
   TCCR1B = 0b00000101; // start the timer with prescalar 1024 of 16 MHz
  }
}

/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

ISR(TIMER1_COMPB_vect){
  if(soft_start_enable == true){
    if(soft_start_activation_flag == true){
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , HIGH);
      soft_start_activation_flag = false;
    }
  }
}
