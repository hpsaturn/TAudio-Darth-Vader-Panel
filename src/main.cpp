// Required Libraries (Download zips and add to the Arduino IDE library).
#include "Arduino.h"
#include <ESP32WifiCLI.hpp> 
#include <gui.hpp>
#include <sound.hpp>

#define TAG "WM8978"


bool setup_mode = false;
int setup_time = 10000;
bool first_run = true;

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

void blink(String opts) {
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  int bright = operands.first().toInt();
  int miliseconds = operands.second().toInt();
  Serial.println("starting Adafruit demo..");

  if (bright >0 && bright<=255) {
    strip.setBrightness(bright);
    Serial.printf("changed brightness to: %i\r\n", bright);
  }
  guiDemo();
}

void radio(String opts) {
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  int volume = operands.first().toInt();
  ESP_LOGI(TAG, "Connected. Starting MP3...");
  audio.setVolume(21);
  audio.connecttohost("http://hestia2.cdnstream.com/1458_128");

  // Volume control.
  volume = volume > 63 || volume == 0 ? 50 : volume;
  dac.setSPKvol(volume);         // for board speaker output (Max 63).
  dac.setHPvol(volume, volume);  // for headphone jack left, right channel.
}

void mp3 (String opts){
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  String path = operands.first();
  // audio.connecttoFS(SD, path.c_str());
  audio.setVolume(21);
  audio.connecttoFS(SD, "/mp3/zerg_online_alert.mp3");
  dac.setSPKvol(50);         // for board speaker output (Max 63).
  dac.setHPvol(50, 50);  // for headphone jack left, right channel.
}

void reboot(String opts){
  ESP.restart();
}

void wcli_exit(String opts) {
  setup_time = 0;
  setup_mode = false;
}

void wcli_setup(String opts) {
  setup_mode = true;
  Serial.println("\r\nSetup Mode Enable (fail-safe mode)\r\n");
}

void setup() {
  Serial.begin(115200);
  Serial.begin(115200); // Optional, you can init it on begin()
  Serial.flush();       // Only for showing the message on serial 
  delay(100);
  guiInit();            // Initialize LED stripe to off
  initAudio();          // Audio drivers only

  wcli.setSilentMode(true);
  wcli.begin();         // Alternatively, you can init with begin(115200) 

 // custom commands:
  wcli.term->add("sleep", &sleep,     "\t<mode> <time> ESP32 will enter to sleep mode");
  wcli.term->add("reboot", &reboot,   "\tperform a ESP32 reboot");
  wcli.term->add("blink", &blink,     "\t<brightness> Adafruit LEDs demo");
  wcli.term->add("radio", &radio,     "\tstarting radio broadcast demo");
  wcli.term->add("mp3", &mp3,     "\t<path> play mp3 from path");
  wcli.term->add("exit", &wcli_exit,  "\texit of the setup mode. AUTO EXIT in 10 seg! :)");
  wcli.term->add("setup", &wcli_setup,"\tTYPE THIS WORD to start to configure the device :D\n");

  // Configuration loop in setup:
  // 10 seconds for reconfiguration (first use case or fail-safe mode for example)
  uint32_t start = millis();
  while (setup_mode || (millis() - start < setup_time)) wcli.loop();
  Serial.println();

  if (setup_time == 0)
    Serial.println("==>[INFO] Settings mode end. Booting..\r\n");
  else
    Serial.println("==>[INFO] Time for initial setup over. Booting..\r\n");

}

void loop() {
  wcli.loop();
  audio.loop();
}
