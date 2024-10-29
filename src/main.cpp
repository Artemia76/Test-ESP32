#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define PIN 48
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYON 100
#define DELAYOFF 1000

uint8_t R,G,B;


void setup() {
  pixels.begin();
}

void loop() {
  pixels.clear();

  for(int i=0; i<NUMPIXELS; i++) {
    R=static_cast<uint8_t>(random(10));
    G=static_cast<uint8_t>(random(10));
    B=static_cast<uint8_t>(random(10));
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
    delay(DELAYON);
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
    delay(DELAYOFF);
  }
}
