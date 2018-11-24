//  globals.c
//
//  global variable definitions for cyborg_vision
//

#include <cv.h>
#include <cv_net.h>
#include <cv_cam.h>


char        *MyName;

HOST_INFO   HostInfo;

CAM_RECORD  *Cameras[MAX_CAMERAS];                  // camera record lists

int         DebugLevel = DEBUG_OFF;

