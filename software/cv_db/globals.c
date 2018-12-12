//  globals.c
//
//  global variable definitions for cyborg_vision
//

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "sensors.h"
#include "db/externs.h"


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
// /
// potentially have different TTLs for different sensors
TTLS SensorTtls[] = { { SENSOR_CAMERA, TTL_SECS_DEFAULT, TTL_USECS_DEFAULT },
                      { SENSOR_RANGE,TTL_SECS_DEFAULT, TTL_USECS_DEFAULT },
                      { SENSOR_ACCELL, TTL_SECS_DEFAULT, TTL_USECS_DEFAULT },
                      { SENSOR_ROLL, TTL_SECS_DEFAULT, TTL_USECS_DEFAULT },
                      { SENSOR_MAGNETIC, TTL_SECS_DEFAULT, TTL_USECS_DEFAULT },
                      { 0, 0} };

/// Global debug level
///
/// DEBUG_OFF, DEBUG_INFO, DEBUG_DETAIL, DEBUG_SUPER
int         DebugLevel = DEBUG_OFF;

/// Global FILE * for debug output
///
/// it's standard error unless it gets as part of the command line (-d option)
FILE        *DebugFP;                               // set to stderr at startup


// Global start time
struct timeval StartTime;


/// Global massage rate reporting cadence
//
// every Nth message print out message ratre stats if DEBUG_INFO or more detailed level specified

int MsgRateReportingCadence = DEF_MSG_RATE_REPORTING_CADENCE;

