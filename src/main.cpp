// Required Libraries (Download zips and add to the Arduino IDE library).
#include "Arduino.h"
#include <ESP32WifiCLI.hpp>
#include <WM8978.h> // https://github.com/CelliesProjects/wm8978-esp32
#include <Audio.h>  // https://github.com/schreibfaul1/ESP32-audioI2S

// T-Audio 1.6 WM8978 I2C pins.
#define I2C_SDA     19
#define I2C_SCL     18

// T-Audio 1.6 WM8978 I2S pins.
#define I2S_BCK     33
#define I2S_WS      25
#define I2S_DOUT    26

// T-Audio 1.6 WM8978 MCLK gpio number
#define I2S_MCLKPIN  0

#define WIFI_RETRY_CONNECTION 4

#define TAG "WM8978"

Audio audio;
WM8978 dac;


int LED_PIN = 13;
bool setup_mode = false;
int setup_time = 10000;
bool first_run = true;

/*********************************************************************
 * Optional callback.
 ********************************************************************/
class mESP32WifiCLICallbacks : public ESP32WifiCLICallbacks {
  void onWifiStatus(bool isConnected) {
    if(isConnected) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }

  void onHelpShow() {
    // Enter your custom help here:
    Serial.println("\r\nCustom commands:\r\n");
    Serial.println("sleep <mode> <time> \tESP32 sleep mode (deep or light)");
    Serial.println("echo \"message\" \t\tEcho the msg. Parameter into quotes");
    Serial.println("setLED <PIN> \t\tconfig the LED GPIO for blink");
    Serial.println("blink <times> <millis> \tLED blink x times each x millis");
    Serial.println("reboot\t\t\tperform a soft ESP32 reboot");
  }

  void onNewWifi(String ssid, String passw) {
  }
};

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
  int times = operands.first().toInt();
  int miliseconds = operands.second().toInt();
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(miliseconds);
    digitalWrite(LED_PIN, LOW);
    delay(miliseconds);
  }
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
  wcli.setCallback(new mESP32WifiCLICallbacks());
  wcli.disableConnectInBoot();
  wcli.setSilentMode(true);
  wcli.begin();         // Alternatively, you can init with begin(115200) 

  // Enter your custom commands:
  wcli.term->add("sleep", &sleep, "\t<mode> <time> ESP32 will enter to sleep mode");
  wcli.term->add("blink", &blink, "\t<times> <millis> LED blink x times each x millis");
  wcli.term->add("reboot", &reboot, "\tperform a ESP32 reboot");

  // Setup wm8978 I2C interface.
  if (!dac.begin(I2C_SDA, I2C_SCL)) {
    ESP_LOGE(TAG, "Error setting up dac: System halted.");
    while (1) delay(100);
  }

  // Select I2S pins
  audio.setPinout(I2S_BCK, I2S_WS, I2S_DOUT);
  audio.i2s_mclk_pin_select(I2S_MCLKPIN);

  // WiFi Settings here.
  ESP_LOGI(TAG, "Connected. Starting MP3...");
  // Enter your Icecast station URL here.
  audio.setVolume(21);
  audio.connecttohost("http://hestia2.cdnstream.com/1458_128");
  // Volume control.
  dac.setSPKvol(30); // Change volume here for board speaker output (Max 63).
  dac.setHPvol(50, 50); // Change volume here for headphone jack left, right channel.
}

void loop() {
  wcli.loop();
  audio.loop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
