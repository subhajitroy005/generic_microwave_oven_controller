/**********************************************************
 * state name:  cb_ui_update
 * parent state call: state_back_op_call
***********************************************************/

void zcd_zerocrossing(){
  if(soft_start_enable == true){
    digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , LOW); // Signal OFF at zero crossing point .. sllightly enable at end part
    OCR1B = 150 - soft_start_counter;
    TCNT1 = 0;
    soft_start_activation_flag = true;
    soft_start_counter++;
    soft_start_activation_flag = true;
    // stop prevention
    if(soft_start_counter == 149) // soft start complte and continious mode to op
      soft_start_counter = 0;
      soft_start_enable = false; // disable softstart
      digitalWrite(output_pin[INDEX_OUT_PIN_MICROWAVE].pin , HIGH);
      soft_start_ui_indication = false; // soft start completed
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
