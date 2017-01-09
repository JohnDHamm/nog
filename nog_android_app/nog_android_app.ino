/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

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
#define BLUEFRUIT_HWSERIAL_NAME      Serial1
/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

#include <Adafruit_NeoPixel.h>
#define PIN                     6
#define NUMPIXELS               30
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// for reading bluetooth data
boolean readCompleted = false;
String fromPhone = "";

//for pattern buttons
int pause = 50; //for chase functions

// ****for Carol of the Bells***********
int redLED = 0;
int blueLED = 0;
int greenLED = 0;
int sixteenthNote = 81;
int eighthNote = 162;

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void) {
  
  while (!Serial);  // required for Flora & Micro
  delay(500);

  // turn off neopixel
  strip.begin(); // This initializes the NeoPixel library.
  for(uint8_t i=0; i<NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0,0,0)); // off
  }
  strip.show();

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit App Controller Example"));
  Serial.println(F("-----------------------------------------"));

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

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

}

/**************************************************************************/
/**************************************************************************/
void loop(void) {
  
  while(ble.available()) {
    char ReadChar = (char)ble.read();

    // Right parentheses ) indicates the end of the string
    if ( ReadChar == ')' ) {
      readCompleted = true;
      break;
    }else{
      Serial.print("readChar: ");Serial.println(ReadChar);
      fromPhone += ReadChar;
      Serial.print("fromPhone: ");Serial.println(fromPhone);
    }
  }
  
  if ( readCompleted == true ) {
    Serial.print("read completed: ");
    Serial.println(fromPhone);
    readCompleted = false;
  
    if ( fromPhone == "song" ) {   
      Serial.println("play song");
      songBells();
    }
               
    if ( fromPhone == "1" ) {
      Serial.println("pattern one");
      chaseBlue();
    }

    if ( fromPhone == "2" ) {
      Serial.println("pattern two");
      chaseGreen();
    }
    
    if ( fromPhone == "3" ) {
      Serial.println("pattern three");
      //chaseGreen();
    }
    
    if ( fromPhone == "4" ) {
      Serial.println("pattern four");
      rainbowShift();
    }
    
    
    Serial.println("done");
    fromPhone = "";
  }
  
}



/* *********FUNCTIONS**************** */
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







void clearLights() {
  for(int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
   }
  strip.show();
}

void songBells() {
  int centerGroup[] = {6, 0,5,10,15,20,25};
  int shortBranchGroup[] = {6, 4,9,14,19,24,29};
  int allInnerGroup[] = {12, 0,5,10,15,20,25, 4,9,14,19,24,29};
  int branchArmsGroup[] = {12, 1,3,6,8,11,13,16,18,21,23,26,28};
  int tipsGroup[] = {6, 2,7,12,17,22,27};
  int branchN[] = {5, 0,1,2,3,4};
  int branchNE[] = {5, 5,6,7,8,9};
  int branchSE[] = {5, 10,11,12,13,14};
  int branchS[] = {5, 15,16,17,18,19};
  int branchSW[] = {5, 20,21,22,23,24};
  int branchNW[] = {5, 25,26,27,28,29};

  clearLights();

  delay(125); //intial delay for music to start on phone app

  //1-7 chunky guitar riff with start measure - blue, shortBranchGroup
  chunkyGuitarStart(670, shortBranchGroup);
  for ( int i = 0; i < 6; i++) {
    chunkyGuitarRiff(670, shortBranchGroup);
  }

  //8-12 chunky guitar with bells - bells: yellow, tips, riff: green, arms
  chunkyGuitarBellsStart(667, tipsGroup, 670, shortBranchGroup);
  for ( int i = 0; i < 3; i++) {
    chunkyGuitarBellsRiff(668, tipsGroup, 670, shortBranchGroup);
  }
  chunkyGuitarBellsEnd(667, tipsGroup, 670, shortBranchGroup);

  //13-15 chunky guitar riff with start measure - blue, shortBranchGroup
  chunkyGuitarStart(670, shortBranchGroup);
  for ( int i = 0; i < 2; i++) {
    chunkyGuitarRiff(670, shortBranchGroup);
  }

  //16-20 chunky guitar with bells - bells: yellow, tips, riff: green, arms
  chunkyGuitarBellsStart(667, tipsGroup, 670, shortBranchGroup);
  for ( int i = 0; i < 3; i++) {
    chunkyGuitarBellsRiff(668, tipsGroup, 670, shortBranchGroup);
  }
  chunkyGuitarBellsEnd(667, tipsGroup, 670, shortBranchGroup);

  //21-24 chunky guitar riff with start measure - blue, shortBranchGroup
  chunkyGuitarStart(670, shortBranchGroup);
  for ( int i = 0; i < 3; i++) {
    chunkyGuitarRiff(670, shortBranchGroup);
  }

  //25-28 main riff with red centerGroup
  for ( int i = 0; i < 4; i++) {
    mainRiff(666, centerGroup);
  }

  //29-32 main riff with white allInnerGroup
  for ( int i = 0; i < 4; i++) {
    mainRiff(125, allInnerGroup);
  }

}


void turnOn(int color, int group[]) {
  setColor(color);
  int groupTotal = group[0];
  for (int i = 1; i <= groupTotal; i++) {
    strip.setPixelColor(group[i], strip.Color(redLED, greenLED, blueLED));
  }
  strip.show();
}

void turnOnAll(int color) {
  setColor(color);
  for (int i = 1; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(redLED, greenLED, blueLED));
  }
  strip.show();
}

void turnOff(int group[]) {
  int groupTotal = group[0];
  for (int i = 1; i <= groupTotal; i++) {
    strip.setPixelColor(group[i], strip.Color(0, 0, 0));
  }
  strip.show();
}



void chunkyGuitarStart(int color, int group) {
  turnOnAll(color);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
  for (int i= 0; i < 4; i++) {
    turnOn(color, group);
    delay(sixteenthNote);
    clearLights();
    delay(sixteenthNote);
  }
}

void chunkyGuitarRiff(int color, int group) {
  turnOn(color, group);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
  for (int i= 0; i < 4; i++) {
    turnOn(color, group);
    delay(sixteenthNote);
    clearLights();
    delay(sixteenthNote);
  }
}

void chunkyGuitarBellsStart(int bellsColor, int bellsGroup, int riffColor, int riffGroup) {
  turnOn(riffColor, riffGroup);
  delay(eighthNote);
  turnOff(riffGroup);
  delay(eighthNote);
  for (int i= 0; i < 2; i++) {
    turnOn(riffColor, riffGroup);
    delay(sixteenthNote);
    turnOff(riffGroup);
    delay(sixteenthNote);
  }
  turnOn(riffColor, riffGroup);
  turnOn(bellsColor, bellsGroup);
  delay(sixteenthNote);
  turnOff(riffGroup);
  delay(sixteenthNote);
  turnOff(bellsGroup);
  turnOn(riffColor, riffGroup);
  delay(sixteenthNote);
  turnOff(riffGroup);
}

void chunkyGuitarBellsRiff(int bellsColor, int bellsGroup, int riffColor, int riffGroup) {
  turnOn(bellsColor, bellsGroup);
  turnOn(riffColor, riffGroup);
  delay(eighthNote);
  turnOff(riffGroup);
  delay(eighthNote);
  turnOff(bellsGroup);
  for (int i= 0; i < 2; i++) {
    turnOn(riffColor, riffGroup);
    delay(sixteenthNote);
    turnOff(riffGroup);
    delay(sixteenthNote);
  }
  turnOn(riffColor, riffGroup);
  turnOn(bellsColor, bellsGroup);
  delay(sixteenthNote);
  turnOff(riffGroup);
  delay(sixteenthNote);
  turnOff(bellsGroup);
  turnOn(riffColor, riffGroup);
  delay(sixteenthNote);
  turnOff(riffGroup); 
}

void chunkyGuitarBellsEnd(int bellsColor, int bellsGroup, int riffColor, int riffGroup) {
  turnOn(bellsColor, bellsGroup);
  turnOn(riffColor, riffGroup);
  delay(eighthNote);
  turnOff(riffGroup);
  delay(eighthNote);
  turnOff(bellsGroup);
  for (int i= 0; i < 4; i++) {
    turnOn(riffColor, riffGroup);
    delay(sixteenthNote);
    turnOff(riffGroup);
    delay(sixteenthNote);
  }  
}


void mainRiff(int color, int group) {
  turnOn(color, group);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
  for (int i = 0; i < 2; i++) {
    turnOn(color, group);
    delay(sixteenthNote);
    clearLights();
    delay(sixteenthNote);
  }
  turnOn(color, group);
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


