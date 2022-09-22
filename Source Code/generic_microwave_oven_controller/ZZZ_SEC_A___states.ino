/**********************************************************
 * state name:  state_init_call
 * parent state call: MAIN
***********************************************************/

void state_init_call(struct application_info* app){

  // All system variable reset
  cb_variable_reset(app);
  
  // IO driver init
  cb_io_driver_int(app);

  // External devices initialization
  cb_external_peripheral_init(app);

  // Update the UI after initialization
  cb_ui_update(app);  
}
