/*
 * ultrasonic ranger code for Garmin LIDAR-lite V3 sensor
 *
 */

#define ID_BIT0_PIN       4
#define ID_BIT1_PIN       3
#define ID_BIT2_PIN       2

#define OFFSET_BIT0_PIN   9
#define OFFSET_BIT1_PIN   8
#define OFFSET_BIT2_PIN   7
#define OFFSET_BIT3_PIN   6
#define OFFSET_BIT4_PIN   5


#define ENABLE_PIN        10
#define PW_PIN            11

#define ACTIVE_PIN        13

#define INTER_READ_PAUSE  100     // 1/10 second

#define SENSOR_STARTUP_DELAY     300 // datasheet says at least 250ms
#define SENSOR_CALIBRATION_DELAY 150 // 2 49ms calibration and first read cycles

#define OUTPUT_BAUD_RATE  115200

#define USEC_PER_CM       10         // 10 usec per cm pulse width


int activeState;


void setup() {
  // id setting pins
  pinMode(ID_BIT0_PIN, INPUT_PULLUP);
  pinMode(ID_BIT1_PIN, INPUT_PULLUP);
  pinMode(ID_BIT2_PIN, INPUT_PULLUP);

  // offset setting pins
  pinMode(OFFSET_BIT0_PIN, INPUT_PULLUP);
  pinMode(OFFSET_BIT1_PIN, INPUT_PULLUP);
  pinMode(OFFSET_BIT2_PIN, INPUT_PULLUP);
  pinMode(OFFSET_BIT3_PIN, INPUT_PULLUP);
  pinMode(OFFSET_BIT4_PIN, INPUT_PULLUP);
  
  
  // ultrasonic sensor control pins
  pinMode(ENABLE_PIN, OUTPUT);     // sets the TRIG_PIN as an Output
  pinMode(PW_PIN, INPUT);          // sets the ECHO_PIN as an Input

  digitalWrite(ENABLE_PIN, HIGH);  // not measuring
  
  // heartbeat LED pin
  pinMode(ACTIVE_PIN, OUTPUT);    // flips each time a range happens

  activeState = 0;
  
  Serial.begin(OUTPUT_BAUD_RATE); // Starts the serial communication

  // wait 250ms for sensor to initialize (we'll wait a little extra)
  delay(SENSOR_STARTUP_DELAY);
}



void loop() {
  int   distanceCm, duration, offset, id;

  // we check the id and offset each time we read the sensor in case it got changed
  //
  // one weirdness - to cut down on parts, the switches will actually change the 
  // input to LOW meaning the pin is "active" (and should be considered a '1' bit
  // for the purpose of setting values

  // get ID - 3 pins

  id = 0;       // clear all bits

  if (digitalRead(ID_BIT0_PIN) == LOW) {
    id = id | 0b00000001;
  }
  if (digitalRead(ID_BIT1_PIN) == LOW) {
    id = id | 0b00000010;
  }
  if (digitalRead(ID_BIT2_PIN) == LOW) {
    id = id | 0b00000100;
  }

  offset = 0;
  
  if (digitalRead(OFFSET_BIT0_PIN) == LOW) {
    offset = offset | 0b00000001;
  }
  if (digitalRead(OFFSET_BIT1_PIN) == LOW) {
    offset = offset | 0b00000010;
  }
  if (digitalRead(OFFSET_BIT2_PIN) == LOW) {
    offset = offset | 0b00000100;
  }
  if (digitalRead(OFFSET_BIT3_PIN) == LOW) {
    offset = offset | 0b00001000;
  }
  if (digitalRead(OFFSET_BIT4_PIN) == LOW) {
    offset = offset | 0b00010000;
  }

  
  // Sets the Enable pin to LOW state to start reading
  digitalWrite(ENABLE_PIN, LOW);

  // Reads the PW_PIN, returns the pulse proportional to
  // sound wave travel time
  duration = pulseIn(PW_PIN, HIGH);
  
  digitalWrite(ENABLE_PIN, HIGH);
    
  // Calculating the distance
  //
  // Datasheet says pulse width is scaled to 147 usec per measured
  // inch of range - converting this to cm (147/2.54) gives
  // 57.8 usec per cm and inverting this give .0172 cm per usec
  //
  // this sensor does the divide by 2 for the distance to and from the
  // target automatically so we don't have to divide it by 2

  distanceCm = (duration / USEC_PER_CM - offset);

  // output string should look like "R <id> x <distancecm (adjusted)>"
  Serial.print("R ");
  Serial.print(id);
  Serial.print(" x ");
  Serial.println(distanceCm);

  if (activeState) {
    digitalWrite(ACTIVE_PIN, LOW);  // turn the light off
    activeState = 0;
  } else {
    digitalWrite(ACTIVE_PIN, HIGH); // turn light on
    activeState = 1;
  }

  delay(INTER_READ_PAUSE);        // wait before the next reading
}
