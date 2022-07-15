#include <Arduino.h>

#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include "FastLED.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "*****";
const char *password = "*****";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#define NUM_LEDS 52
#define LED_PIN 5

const uint8_t FIRST_DIGIT[][8] = {
{
  0b00000000,
  0b01100000,
  0b10100000,
  0b10100000,
  0b10100000,
  0b11100000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00100000,
  0b01100000,
  0b00100000,
  0b00100000,
  0b00100000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b01000000,
  0b10100000,
  0b00100000,
  0b01000000,
  0b11100000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b01100000,
  0b00100000,
  0b11100000,
  0b00100000,
  0b11100000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00100000,
  0b10100000,
  0b11100000,
  0b00100000,
  0b00100000,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b01100000,
  0b10000000,
  0b11100000,
  0b00100000,
  0b11100000,
  0b00000000,
  0b00000000
}};

const uint8_t SECOND_DIGIT[][8] = {
{
  0b00000000,
  0b00001110,
  0b00001010,
  0b00001010,
  0b00001010,
  0b00001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000010,
  0b00000110,
  0b00000010,
  0b00000010,
  0b00000010,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00000100,
  0b00001010,
  0b00000010,
  0b00000100,
  0b00001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001110,
  0b00000010,
  0b00000110,
  0b00000010,
  0b00001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001010,
  0b00001010,
  0b00001110,
  0b00000010,
  0b00000010,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001110,
  0b00001000,
  0b00001110,
  0b00000010,
  0b00001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001110,
  0b00001000,
  0b00001110,
  0b00001010,
  0b00001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001110,
  0b00000010,
  0b00000010,
  0b00000010,
  0b00000010,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001110,
  0b00001010,
  0b00001110,
  0b00001010,
  0b00001110,
  0b00000000,
  0b00000000
},{
  0b00000000,
  0b00001110,
  0b00001010,
  0b00001110,
  0b00000010,
  0b00001110,
  0b00000000,
  0b00000000
}};

const uint8_t INDICATORS[][8] = {
{
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00110000
},{
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00001100
}};

const uint8_t CORRECTIONS[8] = {2, 5, 6, 6, 6, 6, 7, 10};

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(32);

  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.setTimeOffset(7200);
  timeClient.begin();
}

void appendImage(const byte* image) {
  int dot = 0;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      dot = 8 * i + j - CORRECTIONS[i];

      int mask = i % 2 == 0 ? bitRead(image[i], 7 - j) : bitRead(image[i], j);

      if (dot > 0 && mask > 0) {
        leds[dot] = CRGB::Green;
      }
    }
  }

  FastLED.show();
}

void clearDisplay() {
  for (int dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot] = CRGB::Black;
  }

  FastLED.show();
}

void setArrayElement(uint8_t ARR[][8], int index, const byte* value) {
  for(int i = 0; i < 8; i++) {
    ARR[index][i] = value[i];
  }
}

void loop() {
  timeClient.update();

  int hours = timeClient.getHours();
  int hour1 = (int) (hours / 10);
  int hour2 = hours - hour1 * 10;

  int minutes = timeClient.getMinutes();
  int minute1 = (int) (minutes / 10);
  int minute2 = minutes - minute1 * 10;

  clearDisplay();
  appendImage(FIRST_DIGIT[hour1]);
  appendImage(SECOND_DIGIT[hour2]);
  appendImage(INDICATORS[0]);
  delay(1500);

  clearDisplay();
  appendImage(FIRST_DIGIT[minute1]);
  appendImage(SECOND_DIGIT[minute2]);
  appendImage(INDICATORS[1]);
  delay(2000);

  // uint8_t TIME[5][8];
  // setArrayElement(TIME, 0, IMAGES[hour1]);
  // setArrayElement(TIME, 1, IMAGES[hour2]);
  // setArrayElement(TIME, 2, IMAGES[10]);
  // setArrayElement(TIME, 3, IMAGES[minute1]);
  // setArrayElement(TIME, 4, IMAGES[minute2]);

  // for (int digit = 0; digit < 5; digit++) {
  //   displayImage(TIME[digit]);
  //   delay(500);
  // }
}
