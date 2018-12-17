//  sensors.h
//
//  structures function prototypes for storing and
//  manipulating sensor data records

#ifndef _GENERIC_SSS_SENSORS_H_
#define _GENERIC_SSS_SENSORS_H_ 1


// ****************************************************************
// sensor  and sensor list definitions
//

typedef struct sensorConnection {
    /// combined port @ speed - right from command line
    char            *portAtSpeed;
    /// one for each sensor connection
    char            *serialPort;
    int             serialSpeed;
    /// fd for each port
    int             fd;
    /// thread-related storage
    pthread_t       tid;
    pthread_attr_t  attr;
    /// pointer to next;
    struct sensorConnection *next;
} SENSOR_CONN;


// ****************************************************************
// general list stuff
//

/// walks and dumps entire object
void dumpSensors(), dumpSensor();

#endif  /* sensors.h */
