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

#define MAX_SENSOR_VALUES   4

/// offsets for data values for each sensor type
#define CAMERA_X            0
#define CAMERA_Y            1
#define CAMERA_W            2
#define CAMERA_H            3

#define RANGE_X             0

#define ACCELL_X            0
#define ACCELL_Y            1
#define ACCELL_Z            2

#define ROLL_X              0
#define ROLL_Y              1
#define ROLL_Z              2

#define MAGNETIC_X          0
#define MAGNETIC_Y          1
#define MAGNETIC_Z          2




typedef struct sensorRecord {
    /// Sensor type (repeated here because it's convenient to have close
    /// to the union of sensor records
    SENSOR_TYPE         type;
    /// arrival of sensor message (actually insertion time in list)
    struct timeval      time;
    /// rawData values
    ///
    /// camera:
    /// x, y coordinate of target object
    /// w, h of object bounding box
    ///
    /// range:
    /// distance
    ///
    /// accelleration:
    /// x, y, z axis
    ///
    /// roll:
    /// x, y, z
    ///
    /// magnetic:
    ///
    /// x, y, z
    int                 rawData[MAX_SENSOR_VALUES];
    /// filtered value after kalman filtering
    int                 filteredData[MAX_SENSOR_VALUES];
    /// kalman filter gain values (maintained per value)
    int                 gain[MAX_SENSOR_VALUES];
    /// pointer to next sensor record
    struct sensorRecord *next;
} SENSOR_RECORD;


typedef struct sensorSubIdList {
    /// sensor type
    char                   *subId;
    /// pointer to next lower level (sensor data records)
    SENSOR_RECORD          *data;
    /// pointer to next sensor sub id list record
    struct sensorSubIdList *next;
} SENSOR_SUBID_LIST;


typedef struct sensorIdList {
    /// sensor id
    char                    *id;
    /// pointer to next lower level (sensor sub id records)
    SENSOR_SUBID_LIST       *subIds;
    /// pointer to next sensor id list record
    struct sensorIdList     *next;
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
void  sensorRecAdd(SENSOR_TYPE, char *, char *, int, int, int, int);

/// ID, TTL - delete old records for only a specific type of sensor
/// (all instantiations of that sensor)
void  sensorRecPruneBySensorType(SENSOR_TYPE, int);

/// TTL - prune all sensor records older than TTL
/// (all instantiations)
void  sensorRecPruneAll();

/// ID - delete all records for a sensor type and ID
void  sensorRecDeleteById(SENSOR_TYPE, char *);

/// ID, subID - delete all records for a sensor type, ID, and subID
void  sensorRecDeleteByIdSubId(SENSOR_TYPE, char *, char *);

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
int sensorRecGetLatestSubId(SENSOR_TYPE, char *, char *, void *);

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
int sensorRecGetAvgSubId(SENSOR_TYPE, char *, char *, void *);

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

/// walks and dumps entire object
void dumpLists(), dumpListStats();

#endif  /* lists.h */
