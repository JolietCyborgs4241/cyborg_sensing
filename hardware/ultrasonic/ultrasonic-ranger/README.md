#Cyborg-Autonomous Ultrasonic Sensor

##Files
There are several files in this directory:

* Sensor Schematic
   * ExpressSCH from ExpressPCB
       * Free and easy to use schematic design tool
       * Only runs under Windows
* PCB Layout
   * ExpressPCB from ExpressPCB
        * Free and easy to use
        * Decent library of user-define components available
        * Easy to add custom components
        * Low cost and quick turnaround
        * Can get gerber files after placing an order
        * Only runs under Windows
* PDF exports of both of the above
* Ardiuno project file (.ino)

##Code Overview
###Initialization
1. Setup the input and output pins to drive the UHC-SR04 Ultrasonic
1. Clear the flag used to toggle the LED connected to pin 13 (this is a standard Arduino connection on almost all boards to an on-board LED)
1. Set the output speed of the serial power to 115200 baud
###Main Loop
1. Send a short pulse to the ultrasonic module
1. Wait for the response and use the length of the pulse back from the ultrasonic module to do the math to calculate (see the comments in the code)
1. We also blink an on-board LED as a heartbeat
    * Connected to pin D13 on the Arduino
    * Every time through the loop we toggle the state of "active_flag"
   * When it's non-zero, we turn the LED on D13 on
   * When it's zero, we turn that LED off
   * This gives us a nice *heartbeat* that tells us:
       * The module has power
       * The module is running and making measurements
       * All from just by looking at the blinking LED (we'll want to make sure we can see the LED when the sensor is installed and running)
  1. After all of the above, the sensor sleeps for a pre-determined amount of time and repeats the main loop() processing
###Output
1. For now, the sensor will output a string giving the detected distance to whatever it is getting the primary echo from
  * "Distance:  # (cm)"
1. In the future it will output something which identifies the sensor type and specific sensor identifier - something like:
  * "U 2 45" (interpreted as ultrasonic sensor type, #2, detecting a target at 45cm)