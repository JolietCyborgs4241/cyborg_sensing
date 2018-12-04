//  cv_list.h
//
//  structures for storing and manipulating camera data records

#ifndef _CV_LIST_H_
#define _CV_LIST_H_ 1


// ****************************************************************
// camera stuff
//

typedef struct camRecord {
    /// arrival of camera message (actually insertion time in list)
    struct timeval       time;
    /// source camera identifier (right or left)
    char                camera;
    /// x coordinate of object
    int                 x;
    /// y coordinate of object
    int                 y;
    /// width of bounding box
    int                 w;
    /// height of bounding box
    int                 h;
    /// pointer to next camera record
    struct camRecord    *next;
} CAMERA_RECORD;

typedef struct camListHdr {
    /// Identifier of the object being reported
    char                *id;
    /// \brief pointer to individual camera records about this object
    ///
    /// object records from R / L cameras are on separate lists
    CAMERA_RECORD       *recs[NUM_OF_CAMERAS];
    /// pointer to next object header
    struct camListHdr   *next;
} CAMERA_LIST_HDR;


/// Get cam list record for a specific object id
CAMERA_LIST_HDR    *camListGetHdrById(char *);


/// ID, camera, x, y, w, h - add a new object (if needed) and camera record (always; newest at front of list)
int  camRecAdd(char *, char, int, int, int, int);

/// ID, TTL - delete old records for only a specific object id (all cameras)
void  camRecPruneById(char *, int);

/// TTL - prune all camera records older than TTL (regardless of camera or object id)
void  camRecPruneAll(int);

/// ID - delete all records for an id (all cameras)
int  camRecDeleteById(char *);

/// get latest record for an id (both cameras)
///
/// pointer to a 2-element CAMERA_RECORD array
///
/// CAMERA_RECORD array is zeroed
int camRecGetLatest(char *, CAMERA_RECORD *);

/// get average x, y, w, h, values for a specific (both cameras)
int camRecGetAvg(char *, CAMERA_RECORD *);

/// zero-out a CAMERA_RECORD object
void zeroCamRecord(CAMERA_RECORD *);

/// walks and dumps entire object and camera lists
void dumpLists();

/// dump an individual CAMERA_RECORD
void dumpCamRecord(CAMERA_RECORD *);

/// zero an individual CAMERA_RECORD
void zeroCamRecord(CAMERA_RECORD *);




// ****************************************************************
// general sensor stuff
//

typedef struct sensorRangeRec {
    /// distance (in cm) for ranging sensor type
    int                 range;
} SENSOR_RANGE_RECORD;

typedef struct sensorGRec {
    /// acceleration X axis
    int                 x;
    /// acceleration X axis
    int                 y;
    /// acceleration X axis
    int                 z;
} SENSOR_G_RECORD;

typedef struct sensorRollRec {
    /// roll rate X axis
    int                 x;
    /// roll rate X axis
    int                 y;
    /// roll rate X axis
    int                 z;
} SENSOR_ROLL_RECORD;


typedef struct sensorMagRec {
    /// mag inclination X axis
    int                 x;
    /// mag inclination X axis
    int                 y;
    /// mag inclination X axis
    int                 z;
} SENSOR_MAG_RECORD;


typedef struct sensorRecord {
    /// arrival of sensor message (actually insertion time in list)
    struct timeval      time;
    /// union of all non-camera sensor records
    union {
        SENSOR_RANGE_RECORD    range;
        SENSOR_G_RECORD        gForce;
        SENSOR_ROLL_RECORD     roll;
        SENSOR_MAG_RECORD      mag;
    } sensorData;
    /// pointer to next sensor record
    void                *next;
} SENSOR_RECORD;


typedef struct sensorIdRecord {
    /// source sensor identifier
    char                id;
    /// pointer to actual sensor data records
    SENSOR_RECORD       *recs;
    /// pointer to next sensor id record
    struct sensorIdRecord       *next;
} SENSOR_ID_RECORD;


typedef struct sensorListHdr {
    /// type of sensor (see sensors.h)
    char                 sensorType;
    ///  pointer to individual sensor records about this object
    SENSOR_ID_RECORD        *recs;
    /// pointer to next object header
    struct sensorListHdr *next;
} SENSOR_LIST_HDR;





// ****************************************************************
// general list stuff
//


#define LOCK_MAX_ATTEMPTS   20

#define LOCK_USLEEP_TIME    100

#endif  /* cv_list.h */
