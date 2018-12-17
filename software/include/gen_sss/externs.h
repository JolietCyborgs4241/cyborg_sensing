//  generic/externs.h
//
//  global variable external definitions for generic sensor server
//
//  the generic server assumes that the sensor generates appropriate
//  measurements in a form taht can go directly to the database
//

#ifndef _GENERIC_EXTERNS_H_
#define _GENERIC_EXTERNS_H_  1

/// program name (set directly from argv[0])
extern  char        *MyName;

/// network connection for incoming camera messages
extern  HOST_INFO   HostInfo;

/// \brief Global debug level
///
/// 0 - OFF
/// 1 - INFO
/// 2 - DETAILED
extern  int         DebugLevel;

/// Global FP for Debug output
extern  FILE        *DebugFP;

/// Global FP for Logging output
extern  FILE        *LogFP;
extern  char        *LogID;

/// serial port connections
extern  SENSOR_CONN *Sensors;

#endif  /* generic/externs.h */
