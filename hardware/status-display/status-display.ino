#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

/*
 * Status display driver code
 *
 */

#define SERIAL_BAUD_RATE  115200

#define ACTIVE_PIN        13    // heartbeat display

#define NUM_LEDS    10

#define LED_PIN     5
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 10


#define LED_TIMEOUT       3000  // msecs - goes red if no updates

long  timestamps[NUM_LEDS];       // array to track timestamps
int   timeout = LED_TIMEOUT;      // timeout is changable with T command

int activeState;


CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


void
setup() {

 int i;
 
  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
    
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  // all LEDs start at full on red
  
  for (i = 0 ; i < NUM_LEDS ; i++) {
    leds[i].r = 255;
    leds[i].g = 0;
    leds[i].b = 0;

    timestamps[i] = 0;
  }
  
  // heartbeat LED pin
  pinMode(ACTIVE_PIN, OUTPUT);    // flips each time a range happens

  activeState = 0;
  
  Serial.begin(SERIAL_BAUD_RATE); // Starts the serial communication
}




void
updateLEDs() {

#ifdef  DEBUG
  int i;

  for (i = 0 ; i < NUM_LEDS ; i++) {

    Serial.print("LED[");
    Serial.print(i);
    Serial.print("] R:");
    Serial.print(leds[i].red);
    Serial.print(", G:");
    Serial.print(leds[i].green);
    Serial.print(", B:");
    Serial.print(leds[i].blue);
    Serial.print(" TS:");
    Serial.println(timestamps[i]);
  }
#endif

  FastLED.show();
}

int
decodeLEDBrightness(char code) {

int retVal;

  switch (code) {

    case 'X':
      retVal = 0;               // off
      break;

    case 'M':
      retVal = 255;             // max on
      break;

    default:                    // 16 brightness levels
                                // 'a' -> 'p'
      retVal = (code - 'a') * 16 + 1;
  }

  if (retVal > 255) {
    retVal = 255;
  }

  return retVal;
}


#define RED_BYTE      0
#define GREEN_BYTE    1
#define BLUE_BYTE     2


void
loop() {

  int   i, ledOffset, inChar;
  long  now;
  char  colors[3];

  if (Serial.available() > 0) {     // there is data

    inChar = Serial.read();

    switch (inChar) {

      case 'T':                                 // Timeout change
        inChar = Serial.read();                 // get the parameter
        
        if (inChar >= '1' && inChar <= '9') {   // settable from 1-9 seconds
          timeout = (inChar - '0') * 1000;      // convert to binary milliseconds
        }
        break;

      case '0':                                 // LED command
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':

        ledOffset = inChar - '0';               // get LED # (adjust from ASCII)

        Serial.readBytes((char *)&leds[ledOffset].red, 3);    // get the color values
    
/*        colors[RED_BYTE]   = decodeLEDBrightness(colors[RED_BYTE]);   // get brightness
        colors[GREEN_BYTE] = decodeLEDBrightness(colors[GREEN_BYTE]); // save in case packet
        colors[BLUE_BYTE]  = decodeLEDBrightness(colors[BLUE_BYTE]);  // if bad, don't change color
    
        if (Serial.read() == '>') {                   // check for the end char,
                                                      // don't change if not '>'
          leds[ledOffset].red   = colors[RED_BYTE];
          leds[ledOffset].green = colors[GREEN_BYTE];
          leds[ledOffset].blue  = colors[BLUE_BYTE];
*/
          timestamps[ledOffset] = millis();       // set time when last changed
//        }
        break;
    }
  }

  // check for timed out LEDs

  now = millis();
  
  for (i = 0 ; i < NUM_LEDS ; i++) {
    if ((now - timestamps[i]) > timeout) {
      leds[i].red   = 255;  // max red
      leds[i].green = 0;    // and only red
      leds[i].blue  = 0;
    }
  }

  updateLEDs();
  
  if (activeState) {
    digitalWrite(ACTIVE_PIN, LOW);  // turn the light off
    activeState = 0;
  } else {
    digitalWrite(ACTIVE_PIN, HIGH); // turn light on
    activeState = 1;
  }
}
