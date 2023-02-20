/*******************************************************************
 * Darth Vater panel 
 * Author: @hpsaturn
 * 2023
 * 
 * Required Libraries:
 * ----------------------------------------------------------------
 * You only need run PlatformIO, he will download them for you.
 * Old Arduino IDE users: Download zips and add to IDE library.
*******************************************************************/

#include <Arduino.h>
#include <ESP32WifiCLI.hpp> 
#include <OTAHandler.h>
#include <gui.hpp>
#include <sound.hpp>
#include <effects.hpp>
#include <power.hpp>
#include <ota.hpp>

int volume = 17;

bool setup_mode = false;
int setup_time = 10000;
bool first_run = true;

bool touchDetected = false;
void onTouchButton(){ touchDetected = true; }
int  touchThreshold = 27;

void touchth (String opts) {
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  touchThreshold = operands.first().toInt();
  Serial.printf("new touch button threshold\t: %i\r\n",touchThreshold);
  wcli.setInt("ktouchth", touchThreshold);
}

void blink(String opts) {
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  int bright = operands.first().toInt();
  int miliseconds = operands.second().toInt();
  Serial.printf("starting strip demo..\r\n");

  if (bright >0 && bright<=255) {
    strip.setBrightness(bright);
    Serial.printf("changed brightness to\t: %i\r\n", bright);
  }
  guiDemo();
}


void radio(String opts) {
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  String url = operands.first();
  int volume = operands.first().toInt();
  ESP_LOGI(TAG, "Connected. Starting MP3...");
  audioSetVolume(volume);
  if (url.isEmpty())
    audioConnecttohost("http://hestia2.cdnstream.com/1458_128"); 
  else
    audioConnecttohost(url.c_str()); 
}

void mp3 (String opts){
  String path = maschinendeck::SerialTerminal::ParseArgument(opts);
  if (path.length() == 0) {
    Serial.println("Please enter a valid path on the SD");
    return;
  }
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  audioConnecttoSD(path.c_str());
  audioSetVolume(volume);
}

void setVol (String opts){
  maschinendeck::Pair<String, String> operands = maschinendeck::SerialTerminal::ParseCommand(opts);
  volume = operands.first().toInt();
  // Volume control.
  volume = volume > 21 || volume == 0 ? 15 : volume;
  Serial.printf("volume = %d\r\n", volume);
}

void stop (String opts){
  audio.stopSong();
}

void processTouch(){
  if (touchDetected){
    touchDetected = false;
    cancelShutdown();
    if(audio.isRunning()) return;
    String effect = effects[random(EFCOUNT)];
    String path = EFFECTDIR + effect;
    audioConnecttoSD(path.c_str());
    audioSetVolume(volume);
    guiDemo();
  }
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
  Serial.begin(115200); // Optional, you can init it on begin()
  Serial.flush();       // Only for showing the message on serial 
  initAudio();          // Audio drivers only
  guiInit();            // Initialize LED stripe to off
  delay(100);

  wcli.disableConnectInBoot(); 
  wcli.setSilentMode(true);
  wcli.begin();         // Alternatively, you can init with begin(115200) 

 // custom commands:
  wcli.term->add("sleep", &sleep,            "\t<mode> <time> ESP32 will enter to sleep mode");
  wcli.term->add("dsoff", &dsoff,            "\tcancel auto-sleep");
  wcli.term->add("reboot", &reboot,          "\tperform a ESP32 reboot");
  wcli.term->add("blink", &blink,            "\t<brightness> Adafruit LEDs demo");
  wcli.term->add("vol", &setVol,             "\t<0-63> update volume of sound driver");
  wcli.term->add("radio", &radio,            "\t<optional url> starting radio broadcast demo");
  wcli.term->add("mp3", &mp3,                "\t<path> play mp3 from path. Into quotes");
  wcli.term->add("stop", &stop,              "\tstop song playback.");
  wcli.term->add("touchth", &touchth,        "\t<int 0-40> touch button threshold.");
  wcli.term->add("exit", &wcli_exit,         "\texit of the setup mode. AUTO EXIT in 10 seg! :)");
  wcli.term->add("setup", &wcli_setup,       "\tTYPE THIS WORD to start to configure the device :D\n");

  // Configuration loop in setup:
  // 10 seconds for reconfiguration (first use case or fail-safe mode for example)
  // uint32_t start = millis();
  // while (setup_mode || (millis() - start < setup_time)) wcli.loop();
  // Serial.println();

  // if (setup_time == 0)
  //   Serial.println("==>[INFO] Settings mode end. Booting..");
  // else
  //   Serial.println("==>[INFO] Time for initial setup over. Booting..");

  delay(100);
  ota.setCallbacks(new mOTACallbacks());
  ota.setup("VATERP", "VATER32"); 

  delay(100);

  touchThreshold = wcli.getInt("touchth", 27);
  Serial.printf("==>[INFO] Touch button threshold\t: %i\r\n", touchThreshold);
  touchAttachInterrupt(T0, onTouchButton, touchThreshold);
  randomSeed(1);
}

void loop() {
  wcli.loop();
  ota.loop();
  processTouch();
  processShutdown();
}
