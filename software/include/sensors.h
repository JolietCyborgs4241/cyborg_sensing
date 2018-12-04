//	sensors.h
//
//  sensor-related constants and structs
//

#ifndef _SENSORS_H_
#define _SENSORS_H_  1



#define	MAX_SENSOR_READ     (2*1024)




/// camera sensor
///
/// C id thing x y w h
///
#define	SENSOR_CAM          'C'

/// range sensor
///
/// assume all rangers return similar records regardless of technology
///
/// range is reported in cm normalized from the output edge of the robot
///
/// R id d
///
#define SENSOR_RANGE        'R'

/// g-force sensor (3-D)
///
/// G id x y z
///
#define SENSOR_G            'G'

/// roll rate sensor (3-D)
///
/// O id x y z
///
#define SENSOR_ROLL         'O'

/// magnetometer (3-D)
///
/// M id x y z
//
#define SENSOR_MAG          'M'

#endif  /* sensors.h */


