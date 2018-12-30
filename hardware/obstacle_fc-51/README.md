# Cyborg-Autonomous Obstacle Sensor (FC-51)

## Files
There are several files in this directory:

* Sensor Schematic
   * ExpressSCH from ExpressPCB
       * Free and easy to use schematic design tool
       * Only runs under Windows
   * PDF exports of schematic
* Ardiuno project file (.ino)

## Code Overview
### Initialization
1. Setup the input pins to drive the FC-51
1. Setup the input pins we need for the configuration settings
    * 3 pins for "ID" (0-7)
1. Clear the flag used to toggle the LED connected to pin 13 (this is a standard Arduino connection on almost all boards to an on-board LED)
1. Set the output speed of the serial power to 115200 baud
### Main Loop
1. Read the ID values (in case they changed - we permit the sensor to be configured while it's running)
1.Read the sensor status (low or 0 means obstacle was detected)
1. We also blink an on-board LED as a heartbeat
    * Connected to pin D13 on the Arduino
    * We use a counter to slow the heartbeat indication to only once every 25 reads which gives us a 1 second or so duty cycle
   * When it's non-zero, we turn the LED on D13 on
   * When it's zero, we turn that LED off
   * This gives us a nice *heartbeat* that tells us:
       * The module has power
       * The module is running and making measurements
       * All from just by looking at the blinking LED (we'll want to make sure we can see the LED when the sensor is installed and running)
  1. After all of the above, the sensor sleeps for a pre-determined amount of time and repeats the above processing
### Output
Sensor output identifies the sensor type and specific sensor identifier:
* "B 2 x 1" (interpreted as obstacle type, ID "2", subID "x", detecting an obstacle)
##Important Note
This is a binary sensor - it doesn't actually measure a value.  The potentiometer on the sensor sets a distance "threshold" which controls whether the sensor sees an obstacle or not.
The "range" is completely driven by this manual setting and since it uses infrared light to do the detection, the reliability of the obstacle detection (especially the range at which it is detected) is impacted by how reflective the obstacle surface is in terms of infrared reflectivity.

At best, this sensor is good for a constrained situation (like whether something is "loaded" or "positioned" on the robot) and for very short ranges.  It's not a general purpose ranger (which is why I created a new sensor type for it).

At worst, it's useless but it still shows how we can extend our sensor handling to accommodate different sensors.

