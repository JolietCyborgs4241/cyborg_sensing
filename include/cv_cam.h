//	cv_cam.h
//
//  camera-related constants and structs
//

#ifndef _CV_CAM_H_
#define _CV_CAM_H_  1


#include <sys/time.h>


#define	MAX_CAMERA_READ     (2*1024)    // largest record from cameras

#define	MAX_CAMERAS         2           // "Left" and "Right"

#define MAX_CAMERA_RECORDS  5           // save this many of the latest records

#define MIN_CAM_REC_SZ      14          // "L N2 x n n n n"

#define MAX_CAMERA_RECTYP_SZ 8          // really only need 3 (2 + '\0')
#define MAX_CAMERA_ID_SZ    100         // length of id for target

typedef struct {
    time_t      secs;                   // arrival time
    suseconds_t usecs;
    char        recType[MAX_CAMERA_RECTYP_SZ];
    char        id[MAX_CAMERA_ID_SZ];   // matched object
    int         x, y, w, h;             // location and bounding box
} CAM_RECORD;


#define	CAM_LEFT        0
#define	CAM_RIGHT       1
extern  CAM_RECORD  *Cameras[];


#endif  /* cv_cam.h */


