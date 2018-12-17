/*
 * ultrasonic ranger code for MaxBotix MB10xx sensor
 *
 */

#define ID_BIT0_PIN       4
#define ID_BIT1_PIN       3
#define ID_BIT2_PIN       2

#define OFFSET_BIT0_PIN   9

#define ENABLE_PIN        10
#define PW_PIN            11

#define ACTIVE_PIN        13

#define INTER_READ_PAUSE  100     // 1/10 second

#define SENSOR_STARTUP_DELAY     300 // datasheet says at least 250ms
#define SENSOR_CALIBRATION_DELAY 150 // 2 49ms calibration and first read cycles

#define OUTPUT_BAUD_RATE  115200

#define SPEED_OF_SOUND_CM_PER_USEC  0.0172   // cm per usec


int activeState;


void setup() {
  // id setting pins
  pinMode(ID_BIT0_PIN, INPUT_PULLUP);
  pinMode(ID_BIT1_PIN, INPUT_PULLUP);
  pinMode(ID_BIT2_PIN, INPUT_PULLUP);

  // offset setting pins
  
  
  // ultrasonic sensor control pins
  pinMode(ENABLE_PIN, OUTPUT);     // sets the TRIG_PIN as an Output
  pinMode(PW_PIN, INPUT);          // sets the ECHO_PIN as an Input

  digitalWrite(ENABLE_PIN, LOW);
  
  // heartbeat LED pin
  pinMode(ACTIVE_PIN, OUTPUT);    // flips each time a range happens

  activeState = 0;
  
  Serial.begin(OUTPUT_BAUD_RATE); // Starts the serial communication

  // wait 250ms for sensor to initialize (we'll wait a little extra)
  delay(SENSOR_STARTUP_DELAY);
  
  digitalWrite(ENABLE_PIN, HIGH); // start calibration cycle
  delay(SENSOR_CALIBRATION_DELAY);
  digitalWrite(ENABLE_PIN, LOW);
  
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
  
  // Sets the Enable pin to HIGH state to start reading
  digitalWrite(ENABLE_PIN, HIGH);

  // Reads the PW_PIN, returns the pulse proportional to
  // sound wave travel time
  duration = pulseIn(PW_PIN, HIGH);
  
  digitalWrite(ENABLE_PIN, LOW);
    
  // Calculating the distance
  //
  // Datasheet says pulse width is scaled to 147 usec per measured
  // inch of range - converting this to cm (147/2.54) gives
  // 57.8 usec per cm and inverting this give .0172 cm per usec
  //
  // this sensor does the divide by 2 for the distance to and from the
  // target automatically so we don't have to divide it by 2

  distanceCm = duration * SPEED_OF_SOUND_CM_PER_USEC;

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
