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
* This is a 100% hardware device which will be driven off of a Raspberry Pi so any software to drive it will live there and not in this directory

## Hardware Overview
### What's it for?
1. Giving us a way to display simple state information from different components in the autonomous subsystem.
    * We could visually record the robot during competitions and get some feedback as to what is hapenning.
1. It provides us something to display debugging information on as part of the development process (once we get the status-display driving process up and running).
### Power
1. It gets power from the USB hub connecting things together (or from a separate 5vdc power supply)
    * It only uses power and does not communicate over the USB connections
    * All LED control comes from the Raspberry Pi using the 40-pin expansion connection
1. It draws no power from the Raspberry Pi and shares only a grounds on the 40-pin expansion connector on the Raspberry Pi so they have a common reference
### Driving the LEDs
1. The Raspberry Pi drives the LEDs through a driver transistor - one for each LED
1. This takes very little power from the Raspberry Pi and isolates the higher current needed to drive the LEDs from the power feeding the Pi or from loading the GPIO pins on the Pi whic have very limited current drive capability.
1. Since we may want to use a series of different color LEDs to make the different LEDs stand out from each other (since different LEDs will be representing different parts of the autonomous system)
    * Different color LEDs need different driving voltages
    * It's overkill but the design allows using a potentiometer to control the brightness of each individual LED (a *multi-turn* potentiometer even - aren't we special?)
   * Lets us use different color LEDs in different locatiosn o the status-display without having to mess around with current-limiting resistors based on the characteristics of a specific individual LED in an particular position on the panel
   * Easy to adjust to prevent a given LED from over-powering and "glaring out" adjacent LEDs by being too bright (plus it keeps our brightness down so we don't inadvertantly become the target of some other robot's vision system - "Ahhh, we're under attack!")
1. Planning on using big 10mm LEDs to make them more visible in the actual status-display panel
## Software
There will be a software service running on the Raspberry Pi that will drive the LEDs in response to status information from individual components.  We'll probably assign LEDs to speciic services either on an individual basis (just one LED for component X for example) or a group of LEDs (depending on how much information we want to convey from some specific component).

We'll work together to figure out how much useful information we can display on 26 individual LEDs (or maybe fewer).  I wouldn't be surprised if some GPIO pins are used for something on the Raspberry Pi already, like an internal peripheral, so we might not be able to use each one even though it seems like it's available (or really acessible).  If we can't use or don't want to use one, we can turn it's brightness way down or just pull the driving transistor out or even cut the trace to the driving transistor if we can't use that output at all.
