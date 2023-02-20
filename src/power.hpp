void gotToSuspend(int type, int seconds) {
    delay(8);  // waiting for writing msg on serial
    //esp_deep_sleep(1000000LL * DEEP_SLEEP_DURATION);
    esp_sleep_enable_timer_wakeup(1000000LL * seconds);
    if (type == 0) esp_deep_sleep_start();
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
    Serial.println("sleep: invalid option");
  }
}

void reboot(String opts){
  ESP.restart();
}