/*
 * obstacle_fc-51
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

#define SENSE_PIN         10

#define ACTIVE_PIN        13

#define ACTIVE_COUNT      25

#define INTER_READ_PAUSE  50     // 1/20 second

#define OUTPUT_BAUD_RATE  115200



int activeState, activeCount;


void setup() {
  // id setting pins
  pinMode(ID_BIT0_PIN, INPUT_PULLUP);
  pinMode(ID_BIT1_PIN, INPUT_PULLUP);
  pinMode(ID_BIT2_PIN, INPUT_PULLUP);

  // obstacle sensor input pin
  pinMode(SENSE_PIN, INPUT);

  // heartbeat LED pin
  pinMode(ACTIVE_PIN, OUTPUT);    // flips each time a range happens

  activeState = 0;
  
  Serial.begin(OUTPUT_BAUD_RATE); // Starts the serial communication
}



void loop() {
  int   obstacle, id, activeCount;
  
  activeCount = ACTIVE_COUNT;

  while (1) {
  
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

    obstacle = ! digitalRead(SENSE_PIN);    // obstacle if low
  
    // output string should look like "B <id> x <0 or 1 indicating obstacle detected>"
    Serial.print("B ");
    Serial.print(id);
    Serial.print(" x ");
    Serial.println(obstacle);

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
    
    delay(INTER_READ_PAUSE);        // wait before the next reading
  }
}
