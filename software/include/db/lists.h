//  lists.h
//
//  structures function prototypes for storing and
//  manipulating sensor data records

#ifndef _CYBORG_DB_LISTS_H_
#define _CYBORG_DB_LISTS_H_ 1

#include "sensors.h"

// ****************************************************************
// sensor  and sensor list definitions
//

typedef struct sensorCamRec {
    /// x, y coordinate of target object
    int                 x, y;
    /// w, h of object bounding box
    int                 w, h;
} SENSOR_CAMERA_RECORD;


typedef struct sensorGRec {
    /// acceleration X axis
    int                 x;
} SENSOR_DIST_RECORD;


typedef struct sensorGRec {
    /// acceleration X axis
    int                 x;
    /// acceleration Y axis
    int                 y;
    /// acceleration Z axis
    int                 z;
} SENSOR_G_RECORD;


typedef struct sensorRollRec {
    /// roll rate X axis
    int                 x;
    /// roll rate Y axis
    int                 y;
    /// roll rate Z axis
    int                 z;
} SENSOR_ROLL_RECORD;


typedef struct sensorMagRec {
    /// mag inclination X axis
    int                 x;
    /// mag inclination Y axis
    int                 y;
    /// mag inclination Z axis
    int                 z;
} SENSOR_MAG_RECORD;


typedef struct sensorRecord {
    /// arrival of sensor message (actually insertion time in list)
    struct timeval      time;
    /// union of all non-camera sensor records
    union {
        SENSOR_CAM_RECORD      camera;
        SENSOR_DIST_RECORD     dist;
        SENSOR_G_RECORD        gForce;
        SENSOR_ROLL_RECORD     roll;
        SENSOR_MAG_RECORD      mag;
    } sensorData;
    /// pointer to next sensor record
    struct sensorRecord        *next;
} SENSOR_RECORD;


typedef struct sensorSubIdList {
    /// sensor type
    SENSOR_SUB_ID       subId;
    /// pointer to next lower level (sensor data records)
    SENSOR_DATA_LIST    *data;
    /// pointer to next sensor sub id list record
    struct sensorSubIdList  *next;
} SENSOR_SUBID_LIST;


typedef struct sensorIdList {
    /// sensor id
    SENSOR_ID           id;
    /// pointer to next lower level (sensor sub id records)
    SENSOR_SUBID_LIST   *subIds;
    /// pointer to next sensor id list record
    struct sensorIdList  *next;
} SENSOR_ID_LIST;


typedef struct sensorList {
    /// sensor type
    SENSOR_TYPE         type;
    /// pointer to next lower level (starting with sensor id records)
    SENSOR_ID_LIST      *sensors;
    /// pointer to next sensor list record
    struct sensorList   *next;
} SENSOR_LIST;




/// type, ID, subID, i1, i2, i3, i4 - adds a new sensor type (if needed)
/// and sensor record (always; newest at front of list)
int  sensorRecAdd(SENSOR_TYPE, char *, SUB_ID_TYPE, int, int, int, int);

/// ID, TTL - delete old records for only a specific tytpe of sensor
/// (all instantiations of that sensor)
void  sensorRecPruneBySensorType(SENSOR_TYPE, int);

/// TTL - prune all sensor records older than TTL
/// (all instantiations)
void  sensorRecPruneAll(int);

/// ID - delete all records for a sensor type and ID
void  sensorRecDeleteById(SENSOR_TYPE, char *);

/// ID, subID - delete all records for a sensor type, ID, and subID
void  sensorRecDeleteByIdSubId(SENSOR_TYPE, char *, SENSOR_SUBID_TYPE);

/// get latest record for a specific sensor in all subIds
/// of a specific sensor type
///
/// pointer to an array of sensor records will be returned
/// (must be freed when no longer needed)
///
/// count is number of sensor instantiations returned
int sensorRecGetLatest(SENSOR_TYPE, char *, void *);

/// get latest record for a specific sensor and subId
/// of a specific sensor type
///
/// pointer to an array of sensor records will be returned
/// (must be freed when no longer needed)
///
/// count is number of sensor instantiations returned
int sensorRecGetLatestSubId(SENSOR_TYPE, char *, SENSOR_SUBID_TYPE, void *);

/// get average values for a specific sensor in all instantiations
/// of a specific sensor type
///
/// pointer to where an array of sensor records will be returned
/// (must be freed when no longer needed)
///
/// count is number of sensor instantiations returned
/// get average x, y, w, h, values for a specific (both cameras)
int sensorRecGetAvg(SENSOR_TYPE, char *, void *);

/// get average values for a specific sensor and subID
/// of a specific sensor type
///
/// pointer to where an array of sensor records will be returned
/// (must be freed when no longer needed)
///
/// count is number of sensor instantiations returned
/// get average x, y, w, h, values for a specific (both cameras)
int sensorRecGetAvgSubId(SENSOR_TYPE, char *, SENSOR_SUBID_TYPE, void *);

/// zero an individual sensor data record
///
/// sensor type and pointer to record
////
/// zeroes only the fields applicable for that sensor and
/// sets sensor type
void zeroSensorDataRecord(SENSOR_TYPE, SENSOR_RECORD *);





// ****************************************************************
// general list stuff
//

/// walks and dumps entire object and camera lists
void dumpLists();


#define LOCK_MAX_ATTEMPTS   20

#define LOCK_USLEEP_TIME    100

#endif  /* lists.h */
