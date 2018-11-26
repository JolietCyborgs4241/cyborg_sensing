//  cv_list.h
//
//  structures for storing and manipulating camera data records

#ifndef _CV_LIST_H_
#define _CV_LIST_H_ 1


typedef struct camRecord {
    /// arrival of camera message (actually inserion time in list)
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


/// Get cam list ptr for the list
CAMERA_LIST_HDR    *camListGetHdr();

/// Get cam list record for a specific object id
CAMERA_LIST_HDR    *camListGetHdrById(char *);

/// dump an individual CAM_LIST_HDR
void dumpCamListHdr(CAMERA_LIST_HDR *);






/// ID, camera, x, y, w, h - add a new object (if needed) and camera record (always; newest at front of list)
int  camRecAdd(char *, char, int, int, int, int);

/// ID, TTL - delete old records for only a specific object id (all cameras)
int  camRecPruneById(char *, int);

/// TTL - prune all camera records older than TTL (regardless of camera or object id)
int  camRecPrune(int);

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

#endif  /* cv_list.h */
