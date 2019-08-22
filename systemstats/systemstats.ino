#include <FastLED.h>

#define LED_PIN  3

#define COLOR_ORDER GRB
#define CHIPSET     WS2812B

#define BRIGHTNESS 20

// Helper functions for an two-dimensional XY matrix of pixels.
// Simple 2-D demo code is included as well.
//
//     XY(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             No error checking is performed on the ranges of x and y.
//
//     XYsafe(x,y) takes x and y coordinates and returns an LED index number,
//             for use like this:  leds[ XY(x,y) ] == CRGB::Red;
//             Error checking IS performed on the ranges of x and y, and an
//             index of "-1" is returned.  Special instructions below
//             explain how to use this without having to do your own error
//             checking every time you use this function.  
//             This is a slightly more advanced technique, and 
//             it REQUIRES SPECIAL ADDITIONAL setup, described below.


// Params for width and height
const uint8_t kMatrixWidth = 10;
const uint8_t kMatrixHeight = 14;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;
// Set 'kMatrixSerpentineLayout' to false if your pixels are 
// laid out all running the same way, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19
//
// Set 'kMatrixSerpentineLayout' to true if your pixels are 
// laid out back-and-forth, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//                         |
//     9 <  8 <  7 <  6 <  5
//     |
//     |
//    10 > 11 > 12 > 13 > 14
//                        |
//                        |
//    19 < 18 < 17 < 16 < 15
//
// Bonus vocabulary word: anything that goes one way 
// in one row, and then backwards in the next row, and so on
// is call "boustrophedon", meaning "as the ox plows."


// This function will return the right 'led index number' for 
// a given set of X and Y coordinates on your matrix.  
// IT DOES NOT CHECK THE COORDINATE BOUNDARIES.  
// That's up to you.  Don't pass it bogus values.
//
// Use the "XY" function like this:
//
//    for( uint8_t x = 0; x < kMatrixWidth; x++) {
//      for( uint8_t y = 0; y < kMatrixHeight; y++) {
//      
//        // Here's the x, y to 'led index' in action: 
//        leds[ XY( x, y) ] = CHSV( random8(), 255, 255);
//      
//      }
//    }
//
//
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  
  return i;
}


// Once you've gotten the basics working (AND NOT UNTIL THEN!)
// here's a helpful technique that can be tricky to set up, but 
// then helps you avoid the needs for sprinkling array-bound-checking
// throughout your code.
//
// It requires a careful attention to get it set up correctly, but
// can potentially make your code smaller and faster.
//
// Suppose you have an 8 x 5 matrix of 40 LEDs.  Normally, you'd
// delcare your leds array like this:
//    CRGB leds[40];
// But instead of that, declare an LED buffer with one extra pixel in
// it, "leds_plus_safety_pixel".  Then declare "leds" as a pointer to
// that array, but starting with the 2nd element (id=1) of that array: 
//    CRGB leds_with_safety_pixel[41];
//    CRGB* const leds( leds_plus_safety_pixel + 1);
// Then you use the "leds" array as you normally would.
// Now "leds[0..N]" are aliases for "leds_plus_safety_pixel[1..(N+1)]",
// AND leds[-1] is now a legitimate and safe alias for leds_plus_safety_pixel[0].
// leds_plus_safety_pixel[0] aka leds[-1] is now your "safety pixel".
//
// Now instead of using the XY function above, use the one below, "XYsafe".
//
// If the X and Y values are 'in bounds', this function will return an index
// into the visible led array, same as "XY" does.
// HOWEVER -- and this is the trick -- if the X or Y values
// are out of bounds, this function will return an index of -1.
// And since leds[-1] is actually just an alias for leds_plus_safety_pixel[0],
// it's a totally safe and legal place to access.  And since the 'safety pixel'
// falls 'outside' the visible part of the LED array, anything you write 
// there is hidden from view automatically.
// Thus, this line of code is totally safe, regardless of the actual size of
// your matrix:
//    leds[ XYsafe( random8(), random8() ) ] = CHSV( random8(), 255, 255);
//
// The only catch here is that while this makes it safe to read from and
// write to 'any pixel', there's really only ONE 'safety pixel'.  No matter
// what out-of-bounds coordinates you write to, you'll really be writing to
// that one safety pixel.  And if you try to READ from the safety pixel,
// you'll read whatever was written there last, reglardless of what coordinates
// were supplied.

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}


// Demo that USES "XY" follows code below



/*
  Serial Event example

  When new serial data arrives, this sketch adds it to a String.
  When a newline is received, the loop prints the string and clears it.

  A good test for this is to try it with a GPS receiver that sends out
  NMEA 0183 sentences.

  NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
  other ATmega32U4 based boards.

  created 9 May 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SerialEvent
*/

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
//byte value = 0;

byte CPU0 = 0;
byte CPU1 = 0;
byte CPU2 = 0;
byte CPU3 = 0;
byte SWAP = 0;
byte RAMM = 0;
byte HOME = 0;
byte ROOT = 0;


void loop()
{   
  // print the string when a newline arrives:
  if (stringComplete) {
      //Serial.print(inputString);
      if (inputString.startsWith("CPU0")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         CPU0 = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(CPU0, 0);
      }
      if (inputString.startsWith("CPU1")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         CPU1 = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(CPU1, 1);
      }
      if (inputString.startsWith("CPU2")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         CPU2 = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(CPU2, 2);
      }
      if (inputString.startsWith("CPU3")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         CPU3 = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(CPU3, 3);
      }
      if (inputString.startsWith("RAMM")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         RAMM = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(RAMM, 4);
      }
      if (inputString.startsWith("SWAP")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         SWAP = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(SWAP, 5);
      }
      if (inputString.startsWith("HOME")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         HOME = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(HOME, 6);
      }
      if (inputString.startsWith("ROOT")){
         inputString.remove(0, 4);
         // Serial.print(inputString);
         ROOT = inputString.toInt();
         // Serial.println(value);
         // Serial.println(map(value, 0, 100, 0, 14));
         //Serial.flush(); // clear buffer
         line(ROOT, 7);
      }
      if (inputString.startsWith("DONE")){
         FastLED.show();
      }
    // clear the string:
    inputString = "";
    stringComplete = false;
    
    //FastLED.show();
  }
}

void line ( byte data, byte col){
  for( byte y = 0; y < kMatrixHeight; y++) {
    if ( col < 4 ) {
      if (map(data, 0, 100, 0, 14) > y) {
        leds[ XY(9 - col, y)]  = CHSV( map(y, 0, 14, 100, 255), 255, 255);
      }
      else{
        leds[ XY(9 - col, y)]  = CHSV( 0, 0, 0);
      }
    }
    else if (col < 6 ){
      if (map(data, 0, 100, 0, 14) > y) {
        leds[ XY(9 - col, y)]  = CHSV( map(y, 0, 14, 100, 0), 255, 255);
      }
      else{
        leds[ XY(9 - col, y)]  = CHSV( 0, 0, 0);
      }
    }
    else {
      if (map(data, 0, 100, 0, 14) > y) {
        leds[ XY(9 - col, y)]  = CHSV( map(y, 0, 14, 150, 200), 255, 255);
      }
      else{
        leds[ XY(9 - col, y)]  = CHSV( 0, 0, 0);
      }
    }


    
  }
  if (col == 7) {
    FastLED.show();
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


void setup() {
  // initialize FastLED:
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );

  //Turn off all LEDs
  for( byte y = 0; y < kMatrixHeight; y++) {
    for( byte x = 0; x < kMatrixWidth; x++) {
      leds[ XY(x, y)]  = CHSV( 0, 0, 0);
    }
  }
  FastLED.show();

  // initialize serial:
  Serial.begin(19200);
  // reserve 200 bytes for the inputString:
  inputString.reserve(100);
}
