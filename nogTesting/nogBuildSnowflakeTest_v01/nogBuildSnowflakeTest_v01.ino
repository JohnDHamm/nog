#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

int numPixels = 30;
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, PIN, NEO_GRB + NEO_KHZ800);

int redLED = 100;
int blueLED = 0;
int greenLED = 0;
int pause = 100;

void setup() {
  strip.begin();
  for(uint8_t i = 0; i < numPixels; i++) {
  strip.setPixelColor(i, strip.Color(0,0,0)); // off
  }
  strip.show(); // Initialize all pixels to 'off'  
}


void loop() {

  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(redLED, greenLED, blueLED));
    strip.show();
    delay(pause);
  }

  delay(100);
  
  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(pause);
  }
  
}



void setColor(int color) {
  if (color < 256) {
    redLED = color;
    blueLED = color;
    greenLED = color;
  }
  if (color == 666) { //red
    redLED = 255;
    blueLED = 0;
    greenLED = 0;
  }
  if (color == 667) { //yellow
    redLED = 255;
    blueLED = 0;
    greenLED = 255;
  }
  if (color == 668) { //green
    redLED = 0;
    blueLED = 0;
    greenLED = 255;
  }
  if (color == 669) { //aqua
    redLED = 0;
    blueLED = 255;
    greenLED = 255;
  }
  if (color == 670) { //blue
    redLED = 0;
    blueLED = 255;
    greenLED = 0;
  }
  if (color == 671) { //purple
    redLED = 255;
    blueLED = 255;
    greenLED = 0;
  }
}

