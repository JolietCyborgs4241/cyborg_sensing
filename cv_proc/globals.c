//  globals.c
//
//  global variable definitions for cyborg_vision
//

#include <unistd.h>
#include <stdio.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"


/// Program name
///
/// argv[0] in Linux terms
///
/// Used as part of error and warning messages to identify the program
/// which generate the message in case you are looking at multiple
/// debug or log files
char        *MyName;

/// Network connectivity information
///
/// addresses, ports, socket, etc.
HOST_INFO   HostInfo;

/// Time to Live
///
/// How long any sensor data records are kept around before being purged
///
/// Who cares what was seen 30 seconds ago?
///
/// Typical values are in the low integer number of seconds
int         Ttl = TTL_DEFAULT;

/// Global debug level
///
/// DEBUG_OFF, DEBUG_INFO, DEBUG_DETAIL
int         DebugLevel = DEBUG_OFF;

/// Global FILE * for debug output
///
/// it's standard error unless it gets as part of the command line (-d option)
FILE        *DebugFP;                               // set to stderr at startup

