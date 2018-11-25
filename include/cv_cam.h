//	cv_cam.h
//
//  camera-related constants and structs
//

#ifndef _CV_CAM_H_
#define _CV_CAM_H_  1


#include <sys/time.h>


/// Maximum camera read size
#define	MAX_CAMERA_READ     (2*1024)    // largest record from cameras

/// Minimum correct camera message size
#define MIN_CAM_REC_SIZE    14          // "L N2 x n n n n"

/// "Left" camera ID
#define CAMERA_LEFT         'L'
/// "Right" camera ID
#define CAMERA_RIGHT        'R'

#endif  /* cv_cam.h */


