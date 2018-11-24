# cyborg_vision
Vision-related code for FRC #4241 vision efforts

Architecture is a server for each camera that watches the serial camera output and sends it via a UDP packet to the processor.

The processor maintains lists per camera, per identified object with timestamps and coordinate-based values for each record.

A seperate thread prunes the lists based on an overall TTL.

Another thread processes the records per identified object (on request) and creates direction commands an calculates distances to the identified object.

The overall system is designed to operate around a pair of JeVois smart cameras using both the CLI API to control and configure the camers and the serial output providing object-based information.  Overall, the system is farily indepenent of the visual processis strategy used.
