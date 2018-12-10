/*
 * Derived and extended from:
 *
 * Ultrasonic Sensor HC-SR04 and Arduino Tutorial
 *
 * by Dejan Nedelkovski,
 * www.HowToMechatronics.com
 *
 */

#define ID_BIT0_PIN       4
#define ID_BIT1_PIN       3
#define ID_BIT2_PIN       2

#define OFFSET_BIT0_PIN   9

#define TRIG_PIN          10
#define ECHO_PIN          11

#define ACTIVE_PIN        13

#define INTER_READ_PAUSE  250     // 1/4 second

#define OUTPUT_BAUD_RATE  115200

#define SPEED_OF_SOUND_CM_PER_USEC  0.034   // cm per usec


int activeState;


void setup() {
  //i id setting pins
  pinMode(ID_BIT0_PIN, INPUT_PULLUP);
  pinMode(ID_BIT1_PIN, INPUT_PULLUP);
  pinMode(ID_BIT2_PIN, INPUT_PULLUP);

  // offset setting pins
  
  
  // ultrasonic sensor control pins
  pinMode(TRIG_PIN, OUTPUT);      // sets the TRIG_PIN as an Output
  pinMode(ECHO_PIN, INPUT);       // sets the ECHO_PIN as an Input

  // heartbeat LED pin
  pinMode(ACTIVE_PIN, OUTPUT);    // flips each time a range happens

  activeState = 0;
  
  Serial.begin(OUTPUT_BAUD_RATE); // Starts the serial communication
}



void loop() {
  int   distanceCm, duration, offset, id;

  // we check the id and offset each time we read the sensor in case it got changed
  //
  // one weirdness - to cut down on parts, the switches will actually change the 
  // input to LOW meaning the pin is "active" (and shoudl be considered a '1' bit
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

  // Clears the TRIG_PIN
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Sets the TRIG_PIN on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
    
  // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
    
  // Calculating the distance
  //
  // Speed of sound is ~340m / sec or or
  // 0.034cm / usec (microseconds or millionths of a second
  //
  // the duration of the pulse on the incoming ECHO pin ( measured
  // by the pulseIn() function in usecs) is the amount of time from the
  // start of the ultrasonic pulse (which was sent when the TRIG pin
  // went from HIGH to LOW) to it being receive by the ultrasonic
  // receiver
  //
  // since we now that sound travels at ~ 0.034 cm ever usec, we
  // can multiply the value returned by pulseInO() by that value and
  // get the distance the sound covered in cm
  //
  // We have to divide this by 2 because we are actually hearing the
  // ultrasonic echo from the surface we're ranging to which means the
  // sound has actually travelled TWICE the distance to the target (from
  // the sensor, to the target, and then back to the sensor)
  //
  // We know the speed of sound (roughly), the time it took to cover some
  // distance, and then using the power of math, we can calculate how far
  // of a distance the sound wave covered
  //
  distanceCm = duration * SPEED_OF_SOUND_CM_PER_USEC / 2;

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
