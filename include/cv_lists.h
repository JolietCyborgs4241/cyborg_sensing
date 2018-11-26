//  cv_list.h
//
//  structures for storing and manipulating camera data records

#ifndef _CV_LIST_H_
#define _CV_LIST_H_ 1


typedef struct  {
    /// Identifier of the object being reported
    char    *id;        // name of the object being reported
    /// \brief pointer to individual camera records about this object
    ///
    /// object records from both R / L cameras are on this same list
    void    *recs;      // list of records for this id
    /// pointer to next object header
    void    *next;      // next header record
} CAM_LIST_HDR;



typedef struct {
    /// arrival of camera message (actually inserion time in list)
    time_t      secs;                   // arrival time
    suseconds_t usecs;
    /// camera identifier (right or left)
    char        camera;                 // source camera
    /// x coordinate of object
    int         x;
    /// y coordinate of object
    int         y;
    /// width of bounding box
    int         w;
    /// height of bounding box
    int         h;
    /// pointer to next camera record
    void        *next;                  // next camera record
} CAM_RECORD;


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
/// pointer to a 2-element CAM_RECORD array
///
/// CAM_RECORD array is zeroed
int camRecGetLatest(char *, CAM_RECORD *);

/// get average x, y, w, h, values for a specific (both cameras)
int camRecGetAvg(char *, CAM_RECORD *);

/// zero-out a CAM_RECORD object
void zeroCamRecord(CAM_RECORD *);

/// walks and dumps entire object and camera lists
void dumpLists();

/// dump an individual CAM_RECORD
void dumpCamRecord(CAM_RECORD *);

/// zero an individual CAM_RECORD
void zeroCamRecord(CAM_RECORD *);

/// dump an individual CAM_LIST_HDR
void dumpCamListHdr(CAM_LIST_HDR *);

#endif  /* cv_list.h */
