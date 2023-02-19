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

#include "SD.h"
#include "SPI.h"
#include "FS.h"
#include "Ticker.h"

// Digital I/O used
#define SD_CS         13
#define SPI_MOSI      15
#define SPI_MISO      2
#define SPI_SCK       14

Audio audio;
WM8978 dac;


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

void initAudio() {
  // Setup wm8978 I2C interface.
  if (!dac.begin(I2C_SDA, I2C_SCL)) {
    ESP_LOGE(TAG, "Error setting up dac: System halted.");
    while (1) delay(100);
  }
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SD.begin(SD_CS);
  // Select I2S pins
  audio.setPinout(I2S_BCK, I2S_WS, I2S_DOUT);
  audio.i2s_mclk_pin_select(I2S_MCLKPIN);
}