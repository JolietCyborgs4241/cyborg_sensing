/*
 * Derived and extended from:
 *
 * Ultrasonic Sensor HC-SR04 and Arduino Tutorial
 *
 * by Dejan Nedelkovski,
 * www.HowToMechatronics.com
 *
 */
    
#define TRIG_PIN          10
#define ECHO_PIN          11
#define ACTIVE_PIN        13

#define INTER_READ_PAUSE  250     // 1/4 second


long duration;
int  distanceCm;

int activeState;



void setup() {
  pinMode(TRIG_PIN, OUTPUT);      // Sets the TRIG_PIN as an Output
  pinMode(ECHO_PIN, INPUT);       // Sets the ECHO_PIN as an Input

  pinMode(ACTIVE_PIN, OUTPUT);  // flips each time a range happens

  activeState = 0;
  
  Serial.begin(9600); // Starts the serial communication
}



void loop() {
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
  distanceCm = duration*0.034/2;

  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" (cm)");

  if (activeState) {
    digitalWrite(ACTIVE_PIN, LOW);  // turn the light off
    activeState = 0;
  } else {
    digitalWrite(ACTIVE_PIN, HIGH); // turn light on
    activeState = 1;
  }

  delay(INTER_READ_PAUSE);        // wait before the next reading
}
