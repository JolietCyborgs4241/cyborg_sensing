# Vision Testing Setup Details

Below are some photos of the overall testing configuration:

![Camera targets](https://github.com/JolietCyborgs4241/cyborg_sensing/blob/master/r%2Bd/Jevois%20Camera%20Angle%20Test/camera_targets.jpg)

This shows the overall view of the testing configuration showing the red cargo hatch assembly,
the alignment lines for each cargo hatch, and test location markers (labels A, B, C, D, E, and F)
and the marks used for angular orientation of the test trolley.

Here is a closeup of the test locations and angular orientation marks.

![Test locations and angular orientation marks](https://github.com/JolietCyborgs4241/cyborg_sensing/blob/master/r%2Bd/Jevois%20Camera%20Angle%20Test/camera_angles.jpg)

## Camera Mounting

To provide a stable and repeatable platform for maintaining the camera angle and to allow it to be accurately and repeatedly
located over the test locations at different angles, a trolley was built using ball casters.
The use of ball casters simulates the robot movement options available using the "swerve" drive employed for this
years chassis and allowed us to smoothly move the robot in any direction for actual robot motion / video processing 
interaction testing.

This would support the camera at
a height and orientation that we felt was likely to be indicative of how it would be mounted to the final
robot (both in terms of height and location on the robot chassis)

Here is a closeup of the camera trolley with one of the Jevois smart cameras mounted on it:

![Camera trolley](https://github.com/JolietCyborgs4241/cyborg_sensing/blob/master/r%2Bd/Jevois%20Camera%20Angle%20Test/camera_trolley.jpg)

## Dimensions

### Test Point Locations

For each test, the camera was located vertically over one of the test points A through F.
These test points are on a 10" x 10" grid with the closest row 35" from the far end
of the alignment line.

### Camera Location

The camera itself was located with its lense 12" above the floor at angle of approximately 30 degrees
pointed forward and downward.  Tests were done using cameras with viewing angles of both 60 and 90
degrees and are noted as such in the associated spreadsheet.

## Test Results

For each test location, a series of permutations was tested:

* 90 degrees to the target (referred to as our 0 degree baseline)
* -30 degrees to the target (robot turned left - counter-clockwise when viewed from above)
* -15 degrees to the target
* +30 degrees to the target (roboto turned right - clockwise when viewed from above)
* +15 degrees to the target

A screen shot from `jevois-inventor` was captured showing the cameras view (with filtered lines highlighted) as well as
console output displaying camera resolution, line start and end locations, line length, and line angle.
