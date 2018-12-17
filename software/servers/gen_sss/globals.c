//  globals.c
//
//  global variable definitions for gen_sss
//

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

#include "cv.h"
#include "sensors.h"
#include "cv_net.h"
#include "gen_sss/sensors.h"
#include "gen_sss/externs.h"


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
///
/// all sensor instanes send to the same destination
HOST_INFO   HostInfo;

/// Global debug level
///
/// DEBUG_OFF, DEBUG_INFO, DEBUG_DETAIL, DEBUG_SUPER
int         DebugLevel = DEBUG_OFF;

/// Global FILE * for debug output
///
/// it's standard error unless it gets as part of the command line (-d option)
FILE        *DebugFP;

/// Log file
FILE        *LogFP = (FILE *)NULL;
char        *LogID = "gen_sss";

/// Sensor instances
SENSOR_CONN *Sensors = NULL;
