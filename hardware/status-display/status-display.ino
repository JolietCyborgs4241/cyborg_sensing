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


#define ACTIVE_COUNT      250     // update the heartbeat LED this many cycles through while() in loop()

int activeState, activeCount;


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
  
  Serial.begin(SERIAL_BAUD_RATE, SERIAL_8N1); // Starts the serial communication
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



#define RED_BYTE        0
#define GREEN_BYTE      1
#define BLUE_BYTE       2

typedef struct  {
  unsigned char  colors[3];
} CYBORG_PALETTE;


CYBORG_PALETTE  statusPalette[] =  { {   0,   0,   0 },     // off
                                     { 255,   0,   0 },     // red
                                     {   0, 255,   0 },     // green
                                     {   0,   0, 255 },     // blue
                                     {  65,  65,   0 },     // yellow
                                     { 220,   0, 160 },     // purple
                                     { 255,  70,   0 },     // orange
                                     { 255, 255, 255 } };   // white

#define LED_CMD_MASK      0xf0
#define COLOR_MASK        0x0f
#define TIMEOUT_MASK      0x0f

#define TIMEOUT_CMD       0xf0


void
decodeLEDColor(unsigned char cmd, unsigned char *colors) {

      colors[RED_BYTE]   = statusPalette[COLOR_MASK & cmd].colors[RED_BYTE];
      colors[GREEN_BYTE] = statusPalette[COLOR_MASK & cmd].colors[GREEN_BYTE];
      colors[BLUE_BYTE]  = statusPalette[COLOR_MASK & cmd].colors[BLUE_BYTE];
}




void
loop() {

  int   ledOffset, timeoutOffset, inChar;
  long  now;

  timeoutOffset = 0;

  activeCount = ACTIVE_COUNT;

  while (1) {
      if (Serial.available() > 0) {          // there is data

      inChar = Serial.read();

      if ((inChar & LED_CMD_MASK) == TIMEOUT_CMD) {

        timeout = (inChar & TIMEOUT_MASK) * 1000;

      } else {

        ledOffset = (inChar & LED_CMD_MASK) >> 4;

        if (ledOffset < NUM_LEDS) {          // LED range is OK
      
          decodeLEDColor(inChar, &leds[ledOffset].red);

          timestamps[ledOffset] = millis();   // set time when last changed
        }
      }
    }

  // check for timed out LEDs
  //
  // we get here frequently enough that we don't need to check the
  // entire list of LEDs every single time so we keep an offset and
  // check a specific LED each time through the loop
  //
  // this is still sub-second responsive and prevents us from wasting a lot of time
  // going over the whole list every time
  //
  // this gets us back to handling the serial data faster to minimize the chances
  // of overrunning our 64 byte buffer for incoming serial data

    if (timeoutOffset >= NUM_LEDS) {
      timeoutOffset = 0;
    }
  
    now = millis();

    // check the LED pointed to by timeoutOffset this time around
    
    if ((now - timestamps[timeoutOffset]) > timeout) {
      leds[timeoutOffset].red   = 255;  // max red
      leds[timeoutOffset].green = 0;    // and only red
      leds[timeoutOffset].blue  = 0;
    }

    timeoutOffset++;      // look at the next LED next time

    updateLEDs();
  
    if (activeCount) {
      
      activeCount--;
      
    } else {
      
      if (activeState) {
        digitalWrite(ACTIVE_PIN, LOW);  // turn the light off
        activeState = 0;
      } else {
        digitalWrite(ACTIVE_PIN, HIGH); // turn light on
        activeState = 1;
      }

      activeCount = ACTIVE_COUNT;
    }
  }
}
