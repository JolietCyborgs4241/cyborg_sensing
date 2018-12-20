/*
 * Status display driver code
 *
 */

#define OUTPUT_BAUD_RATE  115200

#define ACTIVE_PIN        13    // heartbeat display

#define MAX_LEDS          10

#define LED_TIMEOUT       3000  // msecs - goes red if no updates

typedef struct {
  char  red, green, blue;       // intensity values for each color
  int   timestamp;              // milliseconds at last update
} LED_STATE;

LED_STATE leds[MAX_LEDS];



int activeState;


void
setup() {

  int i;

  // all LEDs start at full on red
  
  for (i = 0 ; i < MAX_LEDS ; i++) {
    leds[i].red = 255;
    leds[i]. green = 0;
    leds[i].blue = 0;

    leds[i].timestamp = 0;
  }
  
  // heartbeat LED pin
  pinMode(ACTIVE_PIN, OUTPUT);    // flips each time a range happens

  activeState = 0;
  
  Serial.begin(OUTPUT_BAUD_RATE); // Starts the serial communication

  updateLEDs();                    // display the initial state
}




void
updateLEDs() {

  
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

    default:                    // 16 brightness level
                                // 'a' -> 'p'
      retVal = (code - 'a') * 16 + 1;
  }

  if (retVal > 255) {
    retVal = 255;
  }

  return retVal;
}





void
loop() {

  int   i, now, ledOffset;
  char  red, green, blue;

  if (Serial.available() > 0) {     // there is data
    
    while (Serial.read() != '<') {  // find the start char - should
                                    // be first unless we're out of sync!
      // keep looking for it...
    }

    ledOffset = Serial.read() - '0';              // get LED # (adjust from ASCII)
    
    red   = decodeLEDBrightness(Serial.read());   // get brightness for each color
    green = decodeLEDBrightness(Serial.read());
    blue  = decodeLEDBrightness(Serial.read());   // save in case this packet is malformed

    if (Serial.read() == '>') {                   // check for the end char,
                                                  // bail if not '>'
      leds[ledOffset].red   = red;
      leds[ledOffset].green = green;              // use the saved values
      leds[ledOffset].blue  = blue;

      leds[ledOffset].timestamp = millis();       // set time when last changed
    }

    // seem to have received a valid status update
  }

  // check for timed out LEDs

  now - millis();
  
  for (i = 0 ; i < MAX_LEDS ; i++) {
    if ((now - leds[i].timestamp) > LED_TIMEOUT) {
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
