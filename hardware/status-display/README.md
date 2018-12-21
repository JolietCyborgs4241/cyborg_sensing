# Cyborg-Autonomous Status Display

## Files
There are several files in this directory:

* Status display Schematic
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

## Hardware Overview
### What's it for?
1. Giving us a way to display simple state information from different components in the autonomous subsystem.
    * We could visually record the robot during competitions and get some feedback as to what is happening.
1. It provides us something to display debugging information on as part of the development process (once we get the status-display driving process up and running).
### Power
1. It gets power to drive the LEDs from the USB hub connecting things together (or from a separate 5vdc power supply) using the USB B connector (the larger square conector)
    * It only uses this for LED power and does not communicate over this USB connection
1. It has a seperate microUSB connector which it usues to communicate to thwe Raspberry Pi (serial over USB)
    * this connection powers the on-board Arduino Nano
### Driving the LEDs
1. The FastLED library is used to drive a set of adderssible RGB LEDs
    * Driven using a single data pin from the Arduino
## Software
There will be a software service running on the Raspberry Pi that will commuinicate with the Arduino Nano and drive the LEDs in response to status information from individual components.  See the overall project README.md for more information on the meaning of the LEDs in terms of specific system status.
