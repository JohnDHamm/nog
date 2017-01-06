#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#include <Adafruit_NeoPixel.h>
#define BLUEFRUIT_HWSERIAL_NAME      Serial1

#define FACTORYRESET_ENABLE     1

#define PIN                     6
#define NUMPIXELS               30

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

int pause = 50; //for chase functions

// ****for Carol of the Bells***********
int redLED = 0;
int blueLED = 0;
int greenLED = 0;
int sixteenthNote = 80;
int eighthNote = 160;
int songLength = 192;

/****************setup****************/
void setup(void)
{
  //while (!Serial);  // required for Flora & Micro
  //delay(500);

  // turn off neopixel
  strip.begin(); // This initializes the NeoPixel library.
  for(uint8_t i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0,0,0)); // off
  }
  strip.show();

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Neopixel Color Picker Example"));
  Serial.println(F("------------------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("***********************"));

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("***********************"));

}


/****************LOOP********************/
void loop(void)
{
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */
  printHex(packetbuffer, len);

  // Color
  if (packetbuffer[1] == 'C') {
    uint8_t red = packetbuffer[2];
    uint8_t green = packetbuffer[3];
    uint8_t blue = packetbuffer[4];
    Serial.print ("RGB #");
    if (red < 0x10) Serial.print("0");
    Serial.print(red, HEX);
    if (green < 0x10) Serial.print("0");
    Serial.print(green, HEX);
    if (blue < 0x10) Serial.print("0");
    Serial.println(blue, HEX);

    for(uint8_t i=0; i<NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(red,green,blue));
    }
    strip.show(); // This sends the updated pixel color to the hardware.
  }

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    if (pressed) {
      Serial.println(" pressed"); 
      if (buttnum == 1) {
        songTest();
      }
      if (buttnum == 3) {
        chaseBlue();   
      }
      if (buttnum == 2) {
        chaseGreen(); 
      }
      /*if (buttnum == 5) {
        rainbowShift(); 
      }*/
    } else {
      Serial.println(" released");
    }

  }
}



/************FUNCTIONS*********************/
void chaseBlue() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 125));
    strip.show();
    delay(pause);
  }
  delay(100);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(pause);
  }
}

void chaseGreen() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 125, 0));
    strip.show();
    delay(pause);
  }
  delay(100);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    strip.show();
    delay(pause);
  }
}

void rainbowShift() {
  int shiftAmount = 5;

  //to red
  for (int c = 1; c < 126; c = c + shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(c, 0, 0));
      strip.show();
    }
  }
  //red to yellow
  for (int c = 1; c < 126; c = c + shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(125, c, 0));
      strip.show();
    }
  }
  //yellow to green
  for (int c = 124; c > -1; c = c - shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(c, 125, 0));
      strip.show();
    }
  }
  //green to aqua
  for (int c = 1; c < 126; c = c + shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, 125, c));
      strip.show();
    }
  }
  //aqua to blue
  for (int c = 124; c > -1; c = c - shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(0, c, 125));
      strip.show();
    }
  }
  //blue to purple
  for (int c = 1; c < 126; c = c + shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(c, 0, 125));
      strip.show();
    }
  }
  //purple to red
  for (int c = 124; c > -1; c = c - shiftAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, strip.Color(125, 0, c));
      strip.show();
    }
  }
}


void clearLights() {
  for(int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
   }
  strip.show();
}

void songTest() {
  int centerGroup[] = {0,5,10,15,20,25};
  int centerGroupLength = 6;
  int shortBranchGroup[] = {4,9,14,19,24,29};
  int branchArmsGroup[] = {1,3,6,8,11,13,16,18,21,23,26,28};
  int tipsGroup[] = {2,7,12,17,22,27};
  int branchN[] = {0,1,2,3,4};
  int branchNE[] = {5,6,7,8,9};
  int branchSE[] = {10,11,12,13,14};
  int branchS[] = {15,16,17,18,19};
  int branchSW[] = {20,21,22,23,24};
  int branchNW[] = {25,26,27,28,29};

  clearLights();

  //test 4 measures of main riff with led#2 as red
  for ( int i = 0; i < 4; i++) {
    songMainRiff(666, centerGroup, centerGroupLength);
  }
}


void turnOn(int color, int group[], int groupLength) {
  setColor(color);
  for (int i = 0; i < groupLength; i++) {
    strip.setPixelColor(group[i], strip.Color(redLED, greenLED, blueLED));
  }
  strip.show();
}

void songMainRiff(int color, int group, int groupLength) {
  turnOn(color, group, groupLength);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
  turnOn(color, group, groupLength);
  delay(sixteenthNote);
  clearLights();
  delay(sixteenthNote);
  turnOn(color, group, groupLength);
  delay(sixteenthNote);
  clearLights();
  delay(sixteenthNote);
  turnOn(color, group, groupLength);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
}



void setColor(int color) {
  if (color < 256) {
    redLED = color;
    blueLED = color;
    greenLED = color;
  }
  if (color == 666) { //red
    redLED = 125;
    blueLED = 0;
    greenLED = 0;
  }
  if (color == 667) { //yellow
    redLED = 125;
    blueLED = 0;
    greenLED = 125;
  }
  if (color == 668) { //green
    redLED = 0;
    blueLED = 0;
    greenLED = 125;
  }
  if (color == 669) { //aqua
    redLED = 0;
    blueLED = 125;
    greenLED = 125;
  }
  if (color == 670) { //blue
    redLED = 0;
    blueLED = 125;
    greenLED = 0;
  }
  if (color == 671) { //purple
    redLED = 125;
    blueLED = 125;
    greenLED = 0;
  }
}


