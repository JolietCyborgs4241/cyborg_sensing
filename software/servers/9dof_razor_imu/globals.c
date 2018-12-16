//  globals.c
//
//  global variable definitions for cyborg_vision
//

#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

#include "cv.h"
#include "sensors.h"
#include "cv_net.h"
#include "9dof/externs.h"


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

/// Global debug level
///
/// DEBUG_OFF, DEBUG_INFO, DEBUG_DETAIL, DEBUG_SUPER
int         DebugLevel = DEBUG_OFF;

/// Global FILE * for debug output
///
/// it's standard error unless it gets as part of the command line (-d option)
FILE        *DebugFP;                               // set to stderr at startup

/// Log file
FILE        *LogFP = (FILE *)NULL;

/// Serial port & speed
char    *SerialPort = NULL;;
int     SerialSpeed = 0;;

/// fd for serial port
int SerialFd = 0;


/// sensor ID and SubID values
char    *SensorId = "";
char    *SensorSubId = "x";

