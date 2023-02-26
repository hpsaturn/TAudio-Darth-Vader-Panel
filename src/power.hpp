#define SECS_TO_DEEP_SLEEP  30

bool auto_sleep = true;    // memory flag (disable only via CLI)
uint_fast64_t loopts = 0;  // last loop timestamp (to perform shutdown)

void turnOffHardware() {
  // adc_power_off();     // maybe it has issues (is deprecated)
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);
}

/**
 * The TTGO T9 v1.0 has a hardware issue:
 * https://github.com/LilyGO/TTGO-TAudio/issues/7
 * This function only will be works with USB connection, with battery
 * after ~minute the board will be power off and only it will be back on with
 * the USB connection :(
*/
void panelShutdown() { 
  turnOffHardware();
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
}

void processShutdown() {
  if (auto_sleep && millis() - loopts > SECS_TO_DEEP_SLEEP * 1000) {
    loopts = millis();
    Serial.println("\nshutdown by inactivity\n");
    delay(100);
    // panelShutdown();  // issue: https://github.com/LilyGO/TTGO-TAudio/issues/7 
                      // After 35 sec the board turn off but we don't wakeup :(
  }
}

void cancelShutdown() {
  loopts = millis();
}

void dsoff(String opts) {
  auto_sleep = false;
}

void gotToSuspend(int type, int seconds) {
    delay(8);  // waiting for writing msg on serial
    //esp_deep_sleep(1000000LL * DEEP_SLEEP_DURATION);
    esp_sleep_enable_timer_wakeup(1000000LL * seconds);
    if (type == 0) panelShutdown();
    else esp_light_sleep_start(); 
}

void sleep(String opts) {
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  int seconds = operands.second().toInt();
  if(operands.first().equals("deep")) {
    Serial.println("\ndeep suspending..");
    gotToSuspend(0, seconds);
  }
  else if(operands.first().equals("light")) {
    Serial.println("\nlight suspending..");
    gotToSuspend(1, seconds);
  }
  else {
    Serial.println("sleep: invalid option (deep/light)");
  }
}

void reboot(String opts){
  ESP.restart();
}
