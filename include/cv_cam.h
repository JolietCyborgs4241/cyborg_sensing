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

#define NUM_OF_CAMERAS      2           // "Right" and "Left"

/// "Left" camera ID
#define CAMERA_LEFT_ID      'L'
/// "Right" camera ID
#define CAMERA_RIGHT_ID     'R'

/// "Left" array offset
#define CAMERA_LEFT_IDX     0
/// ""Right" array offset
#define CAMERA_RIGHT_IDX    1

#endif  /* cv_cam.h */


