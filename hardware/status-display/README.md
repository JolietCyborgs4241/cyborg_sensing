# Cyborg-Autonomous Status Display

## Files
There are several files in this directory:

* Status display Schematic
   * ExpressSCH from ExpressPCB
       * Free and easy to use schematic design tool
       * Only runs under Windows
* PCB Layout (or at least there will be)
   * ExpressPCB from ExpressPCB
        * Free and easy to use
        * Decent library of user-define components available
        * Easy to add custom components
        * Low cost and quick turnaround
        * Can get gerber files after placing an order
        * Only runs under Windows
* PDF exports of both of the above
* This is a 100% hardware device which will be driven off of a Raspberry Pi so any software to drive it will live there and not in this directory

## Hardware Overview
### What's it for?
1. Giving us a way to display simple state information from different components in the autonomous subsystem.
    * We could visually record the robot during competiions and get some feedback as to what is hapenning.
1. It provides us something to display debugging information on as part of the development process.
### Power
1. It gets power from the USB hub connecting things togetehr (or from a separate 5vdc power supply)
    * It only uses power and does not vommunicate over the USB connections
    * All driving comes from the Raspberry Pi using the 40-pin expansion connection
1. It draws no power from the Raspberry Pi and shares only a grounds on the 40-pin expansion connector on the Raspberry Pi so they have a common reference
### Driving the LEDs
1. The Raspberry Pi drives the LEDs through a driver transistor
1. This takes very little power from the Raspberry Pi and isolates the higher current needed to drive the LEDs from the power feeding the Pi.
1. Since we may wsnt to use a series of different color LEDs to make the different LEDs stand out from each other (since different LEDs will be representing different parts of the autonomous system).
    * Different color LEDs need a different driving voltage
    * It's overkill but the design allows using a potentiometer to control the brightness of each individual LED
   * Lets us use different color LEDs indifferent places without having to mess around with current-limiting resistors based on the characteristics of each individual LED
   * Easy to adjust to prevent it from over-power adjacent LEDs from being too bright
1. Planning on using big 10mm to make them more visible
## Software
There will be a software service running on the Raspberry Pi that will drivethe LEDs in response to status information from individual components.  We'll probably assign LEDs to speciic services either on an individual basis (just one for component X) or a group (depending on how much information we want to convey).

We'll work together to figure out how much useful information we can display on 26 individual LEDs (or maybe fewer).  I wouldn't be surprised is some GPIO pins are used for something on the Raspberry Pi already, like an internal peripheral, so we might not be able to use each one.  If we can't use or don't want to use one, we can turn it's brightness way down or just pull the driving transistor out (or even cut the trace to the driving transistor if we can't use that output at all).
