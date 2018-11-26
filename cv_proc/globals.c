//  globals.c
//
//  global variable definitions for cyborg_vision
//

#include <unistd.h>
#include <stdio.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"


char        *MyName;

HOST_INFO   HostInfo;

int         Ttl = TTL_DEFAULT;

int         DebugLevel = DEBUG_OFF;
FILE        *DebugFP;                               // set to stderr at startup

