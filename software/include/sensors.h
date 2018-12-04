//	sensors.h
//
//  sensor-related constants and structs
//

#ifndef _SENSORS_H_
#define _SENSORS_H_  1



/// camera sensor
#define	SENSOR_CAM          'C'

/// range sensor
///
/// assume all rangers return similar records regardless of technology
///
/// range is reported in cm normalized from the output edge of the robot
///
#define SENSOR_RANGE        'U'

/// g-force sensor (3-D)
#define SENSOR_G            'G'

/// roll rate sensor (3-D)
#define SENSOR_ROLL         'R'

/// magnetometer (3-D)
#define SENSOR_MAG          'M'

#endif  /* sensors.h */


