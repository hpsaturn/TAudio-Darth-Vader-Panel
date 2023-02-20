#define SECS_TO_DEEP_SLEEP  60

bool auto_sleep = true;    // memory flag (disable only via CLI)
uint_fast64_t loopts = 0;  // last loop timestamp (to perform shutdown)

void panelShutdown() {
  esp_sleep_enable_touchpad_wakeup();
  esp_deep_sleep_start();
}


void processShutdown() {
  if (auto_sleep && millis() - loopts > SECS_TO_DEEP_SLEEP * 1000) {
    loopts = millis();
    Serial.println("\nshutdown by inactivity\n");
    delay(100);
    panelShutdown();
  }
}

void cancelShutdown() {
  loopts = 0;
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