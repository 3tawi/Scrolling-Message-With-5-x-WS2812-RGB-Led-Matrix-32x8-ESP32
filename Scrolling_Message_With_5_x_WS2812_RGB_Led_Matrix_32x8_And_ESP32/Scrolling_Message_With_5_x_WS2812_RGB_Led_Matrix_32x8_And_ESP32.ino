/*
// - Lien vidéo : https://youtu.be/m0A50SI9s7c
//   REQUIRES the following Arduino libraries:
// - FastLED Library: https://github.com/pixelmatix/FastLED
// - Getting Started ESP32 / ESP32S : https://www.youtube.com
*/

#include <FastLED.h>
#include "GPFont.h"

// Params for width and height
const uint8_t kMatrixWidth = 160;
const uint8_t kMatrixHeight = 8;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define PIN_LEDS 13

const bool    kMatrixSerpentineLayout = true;
String Message = "Great Projects wish you a very nice day.";
int xmsg = 160;

#include "Effects.h"
Effects effects;

void setup() {
  Serial.begin(115200);
  Serial.println("resetting!");
  delay(3000);
  FastLED.addLeds<WS2812B,PIN_LEDS,RGB>(leds,NUM_LEDS);
  FastLED.setBrightness(160);
  effects.setFont(GPFont8x8); 
}


void drawMesg(String textmsg) {
  memset(leds, 0x0, NUM_LEDS*3);
  int text_length = -(textmsg.length() * 8);
  effects.setCursor(xmsg, 0);
  effects.print(textmsg);
  xmsg -= 1;
  if (xmsg < text_length) {
    xmsg = 160;
  }
}
void loop() {
  effects.FillNoise();
  drawMesg(Message);
  FastLED.show();
  EVERY_N_SECONDS(10){
    effects.NoiseVariablesSetup();
  }
  noise_x += 800;
  noise_y += 800;
  noise_z += 800;
  delay(10);
}
