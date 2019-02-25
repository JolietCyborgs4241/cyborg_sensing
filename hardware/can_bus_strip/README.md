# Cyborg CAN Bus iring Strip

## Files
There are several files in this directory:

* PCB Layout
   * Single strip
   * Panelized version consisting of 3 strips on one 21 sq in PCB (includes cut marks as part of the top-side silkscreen)
   * ExpressPCB from ExpressPCB
        * Free and easy to use
        * Decent library of user-define components available
        * Easy to add custom components
        * Low cost and quick turnaround
        * Can get gerber files after placing an order
        * Only runs under Windows
* PDF and other exports of both of the above items

## Hardware Overview
### What's it for?
1. Simplify Talon SRX wiring by eliminating excessive CAN Bus wiring and ssocietd connectors
### Connections and Power
1. The strip is intended to be connected using the Weidmuller connectors at one (when terminating the CAN Bus)
or boths ends (when passing the CAN Bus)
    * Either end has a terminating shunt which when in place, terminates teh CAN Bus at that location via a 120 ohm resistor
    * The strip can be trimmed and the last remaining connector can be used to feed the CAN Bu through the strip (though termination
    will be lost at the trimmed end)
1. The strip can be mounted using #6 screws and spacers or using double sided foam tape
1. No power is needed
## Connecting the Talon SRX
Both of the yellow/green CAN Bus wires are connected by color within the Talon SRX.
This is not needed to pass the CAN bus signal as the CAN standard allows devices to be connected up to 1 meter
from the CAN Bus in what is referred to as a "branch".

For lowest wiring density and to prevent unterminated stubs on the CAN Bus, the Talon SRX can be modified to use only a single yellow/green CAN Bus pair which can connect directly to
CAN Bus Strip.
