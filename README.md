# cyborg_vision
Vision-related code for FRC #4241 vision efforts

Architecture is a server (cv_cam) for each camera that watches the serial camera output and sends it via a UDP packet
to the processor (cv_proc).  Each cv_cam modules passes robot object and visual field location informatation to cv_proc.

The processor maintains lists per camera, per identified object with timestamps and coordinate-based values for each record.
A time-based sequence of camera records is maintained for each obect the cameras report.  A seperate thread prunes the lists
based on an overall Time-To-Live (TTL) value to remove old records but still allow access to the latest information from
each camera as well as an averaged set of values based on the TTL.

Another thread processes the records per identified object (on request) and creates direction commands and calculates
distances to the identified object./  This thread will communicate with the cv_robo module.

A third component (cv_robo) will communicate with cv_proc and work with the robot code to steer, drive, and otherwise
operate the robot based on visual cues.  Cv_robo will act just liike a human-operated joystick and move the robot and
actuate the appropriate functions based on visual cues (just like a human operator would).  The actual robot driving logic
lives here.

The overall system is designed to operate around a pair of JeVois smart cameras using both the CLI API to control and
configure the camers and the serial output providing object-based information.  Overall, the system is farily indepenent
of the visual processis strategy used.
