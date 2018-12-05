//	sensors.h
//
//  sensor-related constants and structs
//

#ifndef _SENSORS_H_
#define _SENSORS_H_  1



#define	MAX_SENSOR_READ     (2*1024)


typedef char                SENSOR_TYPE;


/// camera sensor
///
/// C id subid (really the thing the camera sees) x y w h
///
#define	SENSOR_CAM          'C'

/// distance sensor
///
/// assume all distance sensors return similar records regardless of technology
///
/// distance is reported in cm normalized from the output edge of the robot
///
/// D id subid (likely just a fixed dummy value) d
///
#define SENSOR_DIST         'D'

/// g-force sensor (3-D)
///
/// G id subid (likely just a fixed dummy value) x y z
///
#define SENSOR_G            'G'

/// roll rate sensor (3-D)
///
/// R id subid (likely just a fixed dummy value) x y z
///
#define SENSOR_ROLL         'R'

/// magnetometer (3-D)
///
/// M id subid (likely just a fixed dummy value) x y z
//
#define SENSOR_MAG          'M'

#endif  /* sensors.h */


