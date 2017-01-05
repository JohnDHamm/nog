#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, PIN, NEO_GRB + NEO_KHZ800);

int redLED = 0;
int blueLED = 0;
int greenLED = 0;

int sixteenthNote = 80;
int songLength = 36;

int ctr0 = 1;
int index0 = 0;
int led0[] = {666, 2, 0, 4, 667, 2, 0, 4, 668, 2, 0, 4, 666, 2, 0, 4, 667, 2, 0, 2, 668, 1, 0, 1, 669, 1, 0, 1, 670, 1, 0, 3};

int ctr1 = 1;
int index1 = 0;
int led1[] = {0, 4, 50, 2, 0, 2, 50, 2, 0, 2, 0, 4, 255, 2, 0, 2, 255, 2, 0, 2, 255, 2, 0, 2, 671, 1, 0, 1, 671, 1, 0, 1, 671, 1, 0, 3};

  
void setup() {
  strip.begin();
  for(uint8_t i=0; i<NUMPIXELS; i++) {
  strip.setPixelColor(i, strip.Color(0,0,0)); // off
  }
  strip.show(); // Initialize all pixels to 'off'  
}


void loop() {

  for (int i = 1; i < songLength + 1; i++) {
    
    if (ctr0 == i) {
      setColor(led0[index0]);
      strip.setPixelColor(0, strip.Color(redLED, greenLED, blueLED));
      //strip.setPixelColor(0, strip.Color(led0[index0], led0[index0], led0[index0]));
      ctr0 = ctr0 + led0[index0 + 1];
      index0 = index0 + 2;
    }

    if (ctr1 == i) {
      setColor(led1[index1]);
      strip.setPixelColor(1, strip.Color(redLED, greenLED, blueLED));
      ctr1 = ctr1 + led1[index1 + 1];
      index1 = index1 + 2;
    }

  strip.show();
  delay(sixteenthNote);
    
  }

  delay(1000);
  strip.setPixelColor(0, strip.Color(127, 0, 0));
  strip.setPixelColor(1, strip.Color(127, 0, 0));
  strip.show();
  delay(2000);
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.setPixelColor(1, strip.Color(0, 0, 0));
  strip.show();
  delay(1000);

  ctr0 = 1;
  index0 = 0;
  ctr1 = 1;
  index1 = 0;
  

  
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

