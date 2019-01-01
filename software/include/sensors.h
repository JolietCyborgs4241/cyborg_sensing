//	sensors.h
//
//  sensor-related constants and structs
//

#ifndef _SENSORS_H_
#define _SENSORS_H_  1



#define	MAX_SENSOR_READ     (2*1024)


typedef char                SENSOR_TYPE;

typedef struct ttl_info {
    SENSOR_TYPE sensor;
    int         ttlSecs;
    int         ttlUsecs;
} TTLS;


/// camera sensor
///
/// C id subid (really the thing the camera sees) x y w h
///
#define	SENSOR_CAMERA       'C'

/// range sensor
///
/// assume all range sensors return similar records regardless of technology
///
/// range is reported in cm normalized from the output edge of the robot
///
/// R id subid (likely just a fixed dummy value) d
///
#define SENSOR_RANGE        'R'

/// g-force sensor (3-D)
///
/// G id subid (likely just a fixed dummy value) x y z
///
#define SENSOR_ACCELL       'G'

/// roll rate sensor (3-D)
///
/// O id subid (likely just a fixed dummy value) x y z
///
#define SENSOR_ROLL         'O'

/// magnetometer (3-D)
///
/// M id subid (likely just a fixed dummy value) x y z
//
#define SENSOR_MAGNETIC     'M'

/// obstacle
///
/// B id subid (likely just a fixed dummy value) f ('0' or '1' - '1' detected)
//
#define SENSOR_OBSTACLE     'B'

#endif  /* sensors.h */


