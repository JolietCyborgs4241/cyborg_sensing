//  status/externs.h
//
//  global variable external definitions for status server
//

#ifndef _STATUS_EXTERNS_H_
#define _STATUS_EXTERNS_H_  1

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
extern  char        *SerialPort;
extern  int         SerialFd;

#endif  /* status/externs.h */
