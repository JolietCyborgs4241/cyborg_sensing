//  globals.c
//
//  global variable definitions for status server test
//

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "status/status.h"

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
/// all sensor instances send to the same destination
HOST_INFO   StatusServer;

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
char        *LogID = "status";

/// Serial port settings
char        *SerialPort;        // port@speed
int         SerialFd;

int         LedTtl = DEFAULT_LED_TTL;

int         MaxDelay = 1000;    // msecs

int         RandomCount = 1000;


int MsgRateReportingCadence = DEF_MSG_RATE_REPORTING_CADENCE;


struct  timeval StartTime;
