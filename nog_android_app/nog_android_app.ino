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
char receivedData;
boolean continuePattern = false;
char action;

//for patterns
int pause = 50; //for chase functions
int redLED = 0;
int blueLED = 0;
int greenLED = 0;
int currentRandomLightNum;
int currentRandomColor;
int currentPatternColor = 666;
int innerColor = 0;

// ****for Carol of the Bells***********
int sixteenthNote = 81;
int eighthNote = sixteenthNote * 2;
int quarterNote = eighthNote * 2;
int measure = quarterNote * 3;

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
      receivedData = ReadChar;
      Serial.print("receivedData: ");Serial.println(receivedData);
    }
  }
  
  if ( readCompleted == true ) { //on new character received...
    Serial.print("read completed: ");
    Serial.println(receivedData);
    readCompleted = false;

    switch (receivedData) {
      case 'S':
        Serial.println("play song");
        action = receivedData;
        continuePattern = false;
        songBells();
        break;
      case '1':
        Serial.println("pattern one - blink");
        action = receivedData;
        continuePattern = true;
        break;
      case '2':
        Serial.println("pattern two - chase");
        action = receivedData;
        continuePattern = true;
        break;
      case '3':
        Serial.println("pattern three - rainbow");
        action = receivedData;
        continuePattern = true;
        break;
      case '4':
        Serial.println("pattern four - sparkle");
        action = receivedData;
        continuePattern = true;
        break;
      case 'X':
        Serial.println("stop");
        action = receivedData;
        continuePattern = false;
        clearLights();
        break;
    }
    Serial.println("done");
    receivedData = "";
  }

  if ( continuePattern ) {
    switch ( action ) {
      case '1':
        slowBlink();
        break;
      case '2':
        chase();
        break;
      case '3':
        rainbowBurst();
        break;
      case '4':
        sparkle();
        break;
    }
  }
}

/* *********FUNCTIONS**************** */
void slowBlink() {
  int blinkGroup1[] = {24, 0,1,2,3,5,6,7,8,10,11,12,13,15,16,17,18,20,21,22,23,25,26,27,28};
  int blinkGroup2[] = {6, 4,9,14,19,24,29};
  int blinkInterval = 750;
  //currentPatternColor;
  turnOn(currentPatternColor, blinkGroup1);
  delay(blinkInterval * 2);
  turnOff(blinkGroup1);
  turnOn(currentPatternColor, blinkGroup2);
  delay(blinkInterval);
  turnOff(blinkGroup2);
  currentPatternColor ++;
  checkPatternColor();
}

void checkPatternColor() {
  if (currentPatternColor > 671) {
    currentPatternColor = 666;
  }
}

void chase() {
  setRandomColor();
  setColor(currentRandomColor);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(redLED, greenLED, blueLED));
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

void sparkle() {
  int sparklePause = 20;
  int sparkleColor[] = {50, 50, 200};
  for (int i = 0; i < 100; i++) {
    setRandomLightNum();
    turnOnRGBsingleLED(sparkleColor, currentRandomLightNum);
    delay(sparklePause);
    turnOffSingleLED(currentRandomLightNum);
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
  int allInnerGroup[] = {12, 0,4,5,9,10,14,15,19,20,24,25,29};
  int branchArmsGroup[] = {12, 1,3,6,8,11,13,16,18,21,23,26,28};
  int tipsGroup[] = {6, 2,7,12,17,22,27};
  int branchN[] = {4, 0,1,2,3};
  int branchNE[] = {4, 5,6,7,8};
  int branchSE[] = {4, 10,11,12,13};
  int branchS[] = {4, 15,16,17,18};
  int branchSW[] = {4, 20,21,22,23};
  int branchNW[] = {4, 25,26,27,28};
  
  clearLights();
  delay(125); //intial delay for music to start on phone app

  //1-7 chunky guitar riff with start measure - blue, shortBranchGroup, bass: blue all Inner
  chunkyGuitarStart(670, shortBranchGroup);
  for ( int i = 0; i < 6; i++) {
    chunkyGuitarRiff(670, allInnerGroup, 670, shortBranchGroup); //bass, riff
  }

  //8-12 chunky guitar with bells - bells: yellow, tips, riff: blue, shortBranchGroup, bass: blue all Inner, strings: red/yellow/green/aqua
  chunkyGuitarBellsStart(670, allInnerGroup, 667, tipsGroup, 670, shortBranchGroup); //bass, bells, riff
  for ( int i = 0; i < 3; i++) {
    chunkyGuitarBellsRiff(670, allInnerGroup, 668, tipsGroup, 670, shortBranchGroup, 666 + i, branchArmsGroup);  //bass, bells, riff, strings
  }
  chunkyGuitarBellsEnd(670, allInnerGroup, 667, tipsGroup, 670, shortBranchGroup, 669, branchArmsGroup); //bass, bells, riff, strings

  //13-15 chunky guitar riff with start measure - blue, shortBranchGroup
  chunkyGuitarStart(670, shortBranchGroup);
  for ( int i = 0; i < 2; i++) {
    chunkyGuitarRiff(670, allInnerGroup, 670, shortBranchGroup); //bass, riff
  }

  //16-20 chunky guitar with bells - bells: yellow, tips, riff: blue, shortBranchGroup, bass: blue all Inner, strings: red/yellow/green/aqua
  chunkyGuitarBellsStart(670, allInnerGroup, 667, tipsGroup, 670, shortBranchGroup); //bass, bells, riff
  for ( int i = 0; i < 3; i++) {
    chunkyGuitarBellsRiff(670, allInnerGroup, 668, tipsGroup, 670, shortBranchGroup, 666 + i, branchArmsGroup);  //bass, bells, riff, strings
  }
  chunkyGuitarBellsEnd(670, allInnerGroup, 667, tipsGroup, 670, shortBranchGroup, 669, branchArmsGroup); //bass, bells, riff, strings

  //21-24 chunky guitar riff with start measure - blue, shortBranchGroup
  chunkyGuitarStart(670, shortBranchGroup);
  for ( int i = 0; i < 3; i++) {
    chunkyGuitarRiff(670, allInnerGroup, 670, shortBranchGroup); //bass, riff
  }

  //25-28 high guitar main riff with blue bass shortBranch, red/green centerGroup
  highGuitar(125, tipsGroup, 670, shortBranchGroup, 666, 668, centerGroup); //cymbal, bass, riff (2 color)

  //29-32 high guitar x2 main riff with blue bass tips, red/green allInnerGroup
  highGuitarX2(670, tipsGroup, 666, 668, allInnerGroup); //bass, riff (2 color)

  //33-36 descending high guitar
  descendingGuitar(125, centerGroup, 666, tipsGroup, branchArmsGroup, shortBranchGroup, centerGroup); //cymbal, riff (start color, 4 groups)

  //37-40 ascending guitar (2 measures repeats)
  int color1 = 671;
  int color2 = 667;
  for (int i = 0; i < 2; i++) {
    turnOn(color1, branchN);
    delay(eighthNote);
    turnOn(color1, branchNE);
    delay(eighthNote);
    turnOn(color1, branchSE);
    delay(eighthNote);
    turnOn(color1, branchS);
    delay(eighthNote);
    turnOn(color1, branchSW);
    delay(eighthNote);
    turnOn(color1, branchNW);
    delay(eighthNote);
    
    turnOnSingleLED(color1, 4);
    turnOnSingleLED(color1, 29);
    delay(eighthNote);
    turnOnSingleLED(color1, 9);
    turnOnSingleLED(color1, 24);
    delay(eighthNote);
    turnOnSingleLED(color1, 14);
    turnOnSingleLED(color1, 19);
    delay(quarterNote);
    turnOnAll(color2);
    delay(quarterNote);
    clearLights();
  }
  
  //41-44 ascending guitar repeat (1 measure repeats 3x)
  for (int i = 0; i < 3; i++) {
    turnOn(color2, branchN);
    turnOnSingleLED(color1, 29);
    delay(eighthNote);
    turnOff(branchN);
    turnOffSingleLED(29);

    turnOn(color2, branchNE);
    turnOnSingleLED(color1, 24);
    delay(eighthNote);
    turnOff(branchNE);
    turnOffSingleLED(24);

    turnOn(color2, branchSE);
    turnOnSingleLED(color1, 19);
    delay(eighthNote);
    turnOff(branchSE);
    turnOffSingleLED(19);

    turnOn(color2, branchS);
    turnOnSingleLED(color1, 14);
    delay(eighthNote);
    turnOff(branchS);
    turnOffSingleLED(14);

    turnOn(color2, branchSW);
    turnOnSingleLED(color1, 9);
    delay(eighthNote);
    turnOff(branchSW);
    turnOffSingleLED(9);

    turnOn(color2, branchNW);
    turnOnSingleLED(color1, 4);
    delay(eighthNote);
    turnOff(branchNW);
    turnOffSingleLED(4);
  }

  turnOn(color1, branchSW);
  turnOnSingleLED(color2, 9);
  delay(eighthNote);
  turnOn(color1, branchS);
  turnOnSingleLED(color2, 14);
  delay(eighthNote);
  turnOn(color1, branchSE);
  turnOnSingleLED(color2, 19);
  delay(eighthNote);
  turnOn(color1, branchNE);
  turnOnSingleLED(color2, 24);
  delay(eighthNote);
  turnOn(color1, branchN);
  turnOnSingleLED(color2, 29);
  delay(eighthNote);
  turnOn(color1, branchNW);
  turnOnSingleLED(color2, 4);
  delay(eighthNote);
  
  //45-60
  for ( int i = 0; i < 2; i++) {
    //45-48 descending piano chords
    descendPiano(669, shortBranchGroup, 666, tipsGroup); //riff: aqua,shortBranch - guitar: red, tips
    
    //49-52 descend piano and high guitar
    descendPianoHighGuitar(669, shortBranchGroup, 666, tipsGroup); //riff: aqua,shortBranch - guitar: red, tips
  }

  //61-80 quiet piano main riff, + hi piano, + bells, + strings
  int riffColors[] = {666,668};
  int riffGroup1[] = {3, 0,10,20};
  int riffGroup2[] = {3, 5,15,25};
  int hiPianoColors[] = {666,668,667,670,671,669,667,670,666,668};
  int hiPianoGroup1[] = {3, 4,14,24};
  int hiPianoGroup2[] = {3, 9,19,29};
  int bellColors[] = {671,667,669,668};
  int stringColors[] = {668,670,666,671};
  
  quietPianoRiff(riffColors, riffGroup1, riffGroup2, hiPianoColors, hiPianoGroup1, hiPianoGroup2, bellColors, branchArmsGroup, stringColors, tipsGroup);

  //81-88 screaming guitars main riff
  screamGuitars(667, allInnerGroup, 667, branchArmsGroup, tipsGroup); //guitar, string1 string2
  screamGuitars2(668, allInnerGroup, 668, branchArmsGroup, tipsGroup, 668, tipsGroup); //guitar, string1 string2, dblString

  //89-92 organ main riff
  int sweepColors[] = {669,671,667,670};
  int adjustedSixteenthNote = sixteenthNote;
  for (int i = 0; i < 4; i=i+2) {
    turnOn(sweepColors[i], branchN);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i],4);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i], branchNE);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i],9);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i], branchSE);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i],14);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i], branchS);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i],19);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i], branchSW);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i],24);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i], branchNW);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i],29);
    delay(adjustedSixteenthNote);

    turnOn(sweepColors[i+1], branchN);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i+1],29);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i+1], branchNW);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i+1],24);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i+1], branchSW);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i+1],19);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i+1], branchS);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i+1],14);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i+1], branchSE);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i+1],9);
    delay(adjustedSixteenthNote);
    turnOn(sweepColors[i+1], branchNE);
    delay(adjustedSixteenthNote);
    turnOnSingleLED(sweepColors[i+1],4);
    delay(adjustedSixteenthNote);
  }

  //93-96 organ heavy riff
  clearLights();
  int adjustedEighthNote = eighthNote;
  int burstColors[] = {669,671,667,670};
  for (int i = 0; i < 4; i++) {
    turnOn(burstColors[i], centerGroup);
    delay(adjustedEighthNote);
    turnOff(centerGroup);
    delay(adjustedEighthNote);
    turnOn(burstColors[i], centerGroup);
    delay(adjustedEighthNote);
    turnOn(burstColors[i], shortBranchGroup);
    delay(adjustedEighthNote);
    turnOn(burstColors[i], branchArmsGroup);
    delay(adjustedEighthNote);
    turnOn(burstColors[i], tipsGroup);
    delay(adjustedEighthNote);
    clearLights();
  }

  //97-100 huge hits
  hugeHits();

  //101-105 hold and last huge hit
  bigFinish();
}

void turnOn(int color, int group[]) {
  setColor(color);
  int groupTotal = group[0];
  for (int i = 1; i <= groupTotal; i++) {
    strip.setPixelColor(group[i], strip.Color(redLED, greenLED, blueLED));
  }
  strip.show();
}

void turnOnSingleLED(int color, int lightNum) {
  setColor(color);
  strip.setPixelColor(lightNum, strip.Color(redLED, greenLED, blueLED));
  strip.show();
}

void turnOnAll(int color) {
  setColor(color);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(redLED, greenLED, blueLED));
  }
  strip.show();
}

void turnOnAllRGB(int red, int green, int blue) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}

void turnOnRGBsingleLED(int RGBcolor[], int ledNum) {
  strip.setPixelColor(ledNum, strip.Color(RGBcolor[0], RGBcolor[1], RGBcolor[2]));
  strip.show();
}

void turnOff(int group[]) {
  int groupTotal = group[0];
  for (int i = 1; i <= groupTotal; i++) {
    strip.setPixelColor(group[i], strip.Color(0, 0, 0));
  }
  strip.show();
}

void turnOffSingleLED(int lightNum){
  strip.setPixelColor(lightNum, strip.Color(0, 0, 0));
  strip.show();
}

void chunkyGuitarStart(int color, int group) {
  turnOnAll(125);
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

void chunkyGuitarRiff(int bassColor, int bassGroup, int riffColor, int riffGroup) {
  turnOn(bassColor, bassGroup);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
  for (int i= 0; i < 4; i++) {
    turnOn(riffColor, riffGroup);
    delay(sixteenthNote);
    turnOff(riffGroup);
    delay(sixteenthNote);
  }
}

void chunkyGuitarBellsStart(int bassColor, int bassGroup, int bellsColor, int bellsGroup, int riffColor, int riffGroup) {
  turnOn(riffColor, riffGroup);
  turnOn(bassColor, bassGroup);
  delay(eighthNote);
  turnOff(bassGroup);
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

void chunkyGuitarBellsRiff(int bassColor, int bassGroup, int bellsColor, int bellsGroup, int riffColor, int riffGroup, int stringColor, int stringGroup) {
  turnOn(bellsColor, bellsGroup);
  turnOn(stringColor, stringGroup);
  turnOn(bassColor, bassGroup);
  delay(eighthNote);
  turnOff(bassGroup);
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

void chunkyGuitarBellsEnd(int bassColor, int bassGroup, int bellsColor, int bellsGroup, int riffColor, int riffGroup, int stringColor, int stringGroup) {
  turnOn(bellsColor, bellsGroup);
  turnOn(stringColor, stringGroup);
  turnOn(bassColor, bassGroup);
  delay(eighthNote);
  turnOff(bassGroup);
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

void highGuitar(int cymbalColor, int cymbalGroup, int bassColor, int bassGroup, int riffColor1, int riffColor2, int riffGroup) { //cymbal, bass, riff (2 color)
  //measure 25 has cymbals
  turnOn(cymbalColor, cymbalGroup);
  turnOn(bassColor, bassGroup);
  turnOn(riffColor1, riffGroup);
  delay(eighthNote);
  turnOff(bassGroup);
  delay(eighthNote);
  turnOff(cymbalGroup);
  turnOn(riffColor2, riffGroup);
  delay(eighthNote);
  turnOn(riffColor1, riffGroup);
  delay(eighthNote);
  turnOn(riffColor2, riffGroup);
  delay(quarterNote);
  //measure 26-28 no cymbals
  for (int i = 0; i < 3; i++) {
    turnOn(bassColor, bassGroup);
    turnOn(riffColor1, riffGroup);
    delay(eighthNote);
    turnOff(bassGroup);
    delay(eighthNote);
    turnOn(riffColor2, riffGroup);
    delay(eighthNote);
    turnOn(riffColor1, riffGroup);
    delay(eighthNote);
    turnOn(riffColor2, riffGroup);
    delay(quarterNote);
  }
}

void highGuitarX2(int bassColor, int bassGroup, int riffColor1, int riffColor2, int riffGroup) { //bass, riff (2 color)
  for (int i = 0; i < 4; i++) {
    turnOn(bassColor, bassGroup);
    turnOn(riffColor1, riffGroup);
    delay(eighthNote);
    turnOff(bassGroup);
    delay(eighthNote);
    turnOn(riffColor2, riffGroup);
    delay(eighthNote);
    turnOn(riffColor1, riffGroup);
    delay(eighthNote);
    turnOn(riffColor2, riffGroup);
    delay(quarterNote);
  }
}

void descendingGuitar(int cymbalColor, int cymbalGroup, int riffStartColor, int riffGroup1, int riffGroup2, int riffGroup3, int riffGroup4) { //cymbal, riff
  int currentRiffColor = riffStartColor;
  for (int i = 0; i < 3; i++) {
    turnOn(cymbalColor, cymbalGroup);
    turnOn(currentRiffColor, riffGroup1);
    delay(eighthNote);
    turnOff(riffGroup1);
    delay(eighthNote);
    turnOff(cymbalGroup);
    
    turnOn(currentRiffColor, riffGroup1);
    delay(eighthNote);
    turnOff(riffGroup1);
    turnOn(currentRiffColor, riffGroup2);
    
    delay(eighthNote);
    turnOff(riffGroup2);
    currentRiffColor ++;
    turnOn(currentRiffColor, riffGroup3);
    delay(eighthNote);

    turnOff(riffGroup3);
    currentRiffColor ++;
    turnOn(currentRiffColor, riffGroup4);
    delay(eighthNote);
    currentRiffColor = currentRiffColor - 1;
    turnOff(riffGroup4);
  }
  turnOn(cymbalColor, cymbalGroup);
  turnOn(currentRiffColor, riffGroup1);
  delay(quarterNote);
  turnOff(cymbalColor);
  currentRiffColor ++;
  turnOn(currentRiffColor, riffGroup2);
  delay(eighthNote);
  currentRiffColor ++;
  turnOn(currentRiffColor, riffGroup3);
  delay(eighthNote);
  currentRiffColor = riffStartColor;
  turnOn(currentRiffColor, riffGroup4);
  delay(quarterNote);
  clearLights();
}

void descendPiano(int riffColor, int riffGroup, int guitarColor, int guitarGroup){
  turnOnAll(125);
  delay(eighthNote);
  clearLights();
  delay(eighthNote);
  for (int i = 0; i < 9; i++) {
    turnOn(riffColor, riffGroup);
    delay(eighthNote);
    turnOff(riffGroup);
    delay(eighthNote);
  }
  turnOn(riffColor, riffGroup);
  turnOn(guitarColor, guitarGroup);
  delay(eighthNote);
  turnOff(riffGroup);
  turnOff(guitarGroup);
  delay(eighthNote);
}

void descendPianoHighGuitar(int riffColor, int riffGroup, int guitarColor, int guitarGroup) {
  for (int i = 0; i < 12; i++) {
    turnOn(riffColor, riffGroup);
    turnOn(guitarColor, guitarGroup);
    delay(eighthNote);
    turnOff(riffGroup);
    turnOff(guitarGroup);
    delay(eighthNote);
  }
}

void quietPianoRiff(int riffColors[], int riffGroup1, int riffGroup2, int hiPianoColors[], int hiPianoGroup1, int hiPianoGroup2, int bellColors[], int bellGroup, int stringColors[], int stringGroup) {
  //61-63
  for (int i = 0; i < 3; i++) {
    turnOn(riffColors[0], riffGroup1);
    turnOn(riffColors[1], riffGroup2);
    delay(quarterNote);
    turnOn(riffColors[1], riffGroup1);
    turnOn(riffColors[0], riffGroup2);
    delay(eighthNote);
    turnOn(riffColors[0], riffGroup1);
    turnOn(riffColors[1], riffGroup2);
    delay(eighthNote);
    turnOn(riffColors[1], riffGroup1);
    turnOn(riffColors[0], riffGroup2);
    delay(quarterNote);
  }
  //64
  turnOn(riffColors[0], riffGroup1);
  turnOn(riffColors[1], riffGroup2);
  delay(quarterNote);
  turnOn(riffColors[1], riffGroup1);
  turnOn(riffColors[0], riffGroup2);
  delay(eighthNote);
  turnOn(riffColors[0], riffGroup1);
  turnOn(riffColors[1], riffGroup2);
  delay(eighthNote);
  turnOn(riffColors[1], riffGroup1);
  turnOn(riffColors[0], riffGroup2);
  turnOn(hiPianoColors[0], hiPianoGroup1);
  turnOn(hiPianoColors[1], hiPianoGroup2);
  delay(quarterNote);
  //65-72
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 8; j = j + 2) {
      turnOn(riffColors[0], riffGroup1);
      turnOn(riffColors[1], riffGroup2);
      turnOn(hiPianoColors[j+1], hiPianoGroup1);
      turnOn(hiPianoColors[j], hiPianoGroup2);
      delay(quarterNote);
      turnOn(riffColors[1], riffGroup1);
      turnOn(riffColors[0], riffGroup2);
      delay(eighthNote);
      turnOn(riffColors[0], riffGroup1);
      turnOn(riffColors[1], riffGroup2);
      delay(eighthNote);
      turnOn(riffColors[1], riffGroup1);
      turnOn(riffColors[0], riffGroup2);
      turnOn(hiPianoColors[j+2], hiPianoGroup1);
      turnOn(hiPianoColors[j+3], hiPianoGroup2);
      delay(quarterNote);
    }
  }
  //73-76 add bells
  for (int j = 0; j < 8; j = j + 2) {
    turnOn(riffColors[0], riffGroup1);
    turnOn(riffColors[1], riffGroup2);
    turnOn(hiPianoColors[j+1], hiPianoGroup1);
    turnOn(hiPianoColors[j], hiPianoGroup2);
    turnOn(bellColors[j/2], bellGroup);
    delay(quarterNote);
    turnOn(riffColors[1], riffGroup1);
    turnOn(riffColors[0], riffGroup2);
    delay(eighthNote);
    turnOn(riffColors[0], riffGroup1);
    turnOn(riffColors[1], riffGroup2);
    delay(eighthNote);
    turnOn(riffColors[1], riffGroup1);
    turnOn(riffColors[0], riffGroup2);
    turnOn(hiPianoColors[j+2], hiPianoGroup1);
    turnOn(hiPianoColors[j+3], hiPianoGroup2);
    delay(quarterNote);
  }
  //77-80 add strings
  for (int j = 0; j < 8; j = j + 2) {
    turnOn(riffColors[0], riffGroup1);
    turnOn(riffColors[1], riffGroup2);
    turnOn(hiPianoColors[j+1], hiPianoGroup1);
    turnOn(hiPianoColors[j], hiPianoGroup2);
    turnOn(bellColors[j/2], bellGroup);
    turnOn(stringColors[j/2], stringGroup);
    delay(quarterNote);
    turnOn(riffColors[1], riffGroup1);
    turnOn(riffColors[0], riffGroup2);
    delay(eighthNote);
    turnOn(riffColors[0], riffGroup1);
    turnOn(riffColors[1], riffGroup2);
    delay(eighthNote);
    turnOn(riffColors[1], riffGroup1);
    turnOn(riffColors[0], riffGroup2);
    turnOn(hiPianoColors[j+2], hiPianoGroup1);
    turnOn(hiPianoColors[j+3], hiPianoGroup2);
    delay(quarterNote - 50);
  }
  clearLights();
}

void screamGuitars(int guitarColor, int guitarGroup, int stringColor, int stringGroup1, int stringGroup2){
  //81-84
  int adjustedQuarterNote = quarterNote - 50;
  int adjustedEighthNote = eighthNote - 10;
  for (int i = 0; i < 4; i++) { 
    turnOn(guitarColor, guitarGroup);
    delay(adjustedQuarterNote);
    turnOff(guitarGroup);
    turnOn(stringColor, stringGroup1);
    delay(adjustedEighthNote);
    turnOff(stringGroup1);
    turnOn(stringColor, stringGroup2);
    delay(adjustedEighthNote);
    turnOff(stringGroup2);
    turnOn(stringColor, stringGroup1);
    delay(adjustedEighthNote);
    turnOff(stringGroup1);
    delay(adjustedEighthNote);
  }
}

void screamGuitars2(int guitarColor, int guitarGroup, int stringColor, int stringGroup1, int stringGroup2, int dblStringColor, int dblStringGroup){
  //85-88
  int adjustedQuarterNote = quarterNote - 30;
  int adjustedEighthNote = eighthNote - 5;
  for (int i = 0; i < 4; i++) {
    clearLights();
    turnOn(guitarColor, guitarGroup);
    turnOn(dblStringColor, dblStringGroup);
    delay(adjustedQuarterNote);
    turnOff(guitarGroup);
    turnOff(dblStringGroup);
    turnOn(stringColor, stringGroup1);
    delay(adjustedEighthNote);
    turnOff(stringGroup1);
    turnOn(stringColor, stringGroup2);
    delay(adjustedEighthNote);
    turnOff(stringGroup2);
    turnOn(stringColor, stringGroup1);
    delay(adjustedEighthNote);
    turnOff(stringGroup1);
    delay(adjustedEighthNote);
  }
}

void hugeHits() {
  int currentRed = 255;
  int currentGreen = 255;
  int currentBlue = 0;
  int interval = 16;
  int decrement = 256/interval;
  //measure 97 -yellow fade
  for (int i = 0; i < interval; i++) {
    currentRed = currentRed - decrement;
    currentGreen = currentGreen - decrement;
    turnOnAllRGB(currentRed, currentGreen, currentBlue);
    delay(measure / interval);
  }
  //measure 98 -blue fade
  currentBlue = 255;
  currentRed = 0;
  currentGreen = 0;
  for (int i = 0; i < interval; i++) {
    currentBlue = currentBlue - decrement;
    turnOnAllRGB(currentRed, currentGreen, currentBlue);
    delay(measure / interval);
  }
  //measure 99 -red fade
  currentBlue = 0;
  currentRed = 255;
  currentGreen = 0;
  for (int i = 0; i < interval; i++) {
    currentRed = currentRed - decrement;
    turnOnAllRGB(currentRed, currentGreen, currentBlue);
    delay(measure / interval);
  }
  //measure 100 -green fade
  currentBlue = 0;
  currentRed = 0;
  currentGreen = 255;
  for (int i = 0; i < interval; i++) {
    currentGreen = currentGreen - decrement;
    turnOnAllRGB(currentRed, currentGreen, currentBlue);
    delay(measure / interval);
  }
}

void bigFinish() {
  turnOnAll(125);
  delay(sixteenthNote);
  clearLights();
  int duration = (measure * 4) - sixteenthNote;
  int interval = 100;
  int currentLight;
  //101-104
  for (int i = 0; i < interval; i++) {
    //random light number - turn on/off
    setRandomLightNum();
    setRandomColor();
    turnOnSingleLED(currentRandomColor, currentRandomLightNum);
    delay(duration / interval);
    turnOffSingleLED(currentRandomLightNum);
  }
  //105
  turnOnAll(125);
  delay(measure);
  clearLights();
}

void setRandomLightNum() {
  currentRandomLightNum = random(30);
  //Serial.println("random led: ");Serial.print(currentRandomLightNum);
}

void setRandomColor() {
  currentRandomColor = random(666, 672);
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
  //start with all red
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(125, 0, 0));
    strip.show();
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

void rainbowBurst() {
  int shiftAmount = 2;
  int colorOffset = 84;
  int centerGroup[] = {6, 0,5,10,15,20,25};
  int shortBranchGroup[] = {6, 4,9,14,19,24,29};
  int branchArmsGroup[] = {12, 1,3,6,8,11,13,16,18,21,23,26,28};
  int tipsGroup[] = {6, 2,7,12,17,22,27};

  turnOnRainbowGroup(innerColor, centerGroup);
  //delay(100);
  Serial.print("innerColor: ");Serial.println(innerColor);
  int shortBranchColor = innerColor + colorOffset;
  if (shortBranchColor > 749) {shortBranchColor -= 750; }
  Serial.print("shortBranchColor: ");Serial.println(shortBranchColor);
  turnOnRainbowGroup(shortBranchColor, shortBranchGroup);
  //delay(100);
  int armsBranchColor = shortBranchColor + colorOffset;
  if (armsBranchColor > 749) { armsBranchColor -= 750; }
  Serial.print("armsBranchColor: ");Serial.println(armsBranchColor);
  turnOnRainbowGroup(armsBranchColor, branchArmsGroup);
  //delay(100);
  int tipsColor = armsBranchColor + colorOffset;
  if (tipsColor > 749) { tipsColor -= 750; }
  Serial.print("stipsColor: ");Serial.println(tipsColor);
  turnOnRainbowGroup(tipsColor, tipsGroup);
  //delay(100);
  innerColor += shiftAmount;
  if (innerColor > 749) { innerColor -= 750; }
  Serial.print("innerColor: ");Serial.println(innerColor);
}

void turnOnRainbowGroup(int color, int group[]) {
  setRainbowColor(color);
  int groupTotal = group[0];
  for (int i = 1; i <= groupTotal; i++) {
    strip.setPixelColor(group[i], strip.Color(redLED, greenLED, blueLED));
  }
  strip.show();
}

void setRainbowColor(int c) {
  if (c < 126) { //red to yellow
    redLED = 125;
    greenLED = c;
    blueLED = 0;
  }
  else if (c < 251) { //yellow to green
    redLED = 250 - c;
    greenLED = 125;
    blueLED = 0;
  }
  else if (c < 376) { //green to aqua
    redLED = 0;
    greenLED = 125;
    blueLED = c - 250;
  }
  else if (c < 501) { //aqua to blue
    redLED = 0;
    greenLED = 500 - c;
    blueLED = 125;
  }
  else if (c < 676) { //blue to purple
    redLED = c - 500;
    greenLED = 0;
    blueLED = 125;
  }
  else { //purple to red
    redLED = 125;
    greenLED = 0;
    blueLED = 750 - c;
  }
  Serial.print("RGB: ");Serial.print(redLED);Serial.print(greenLED);Serial.println(blueLED);
}

