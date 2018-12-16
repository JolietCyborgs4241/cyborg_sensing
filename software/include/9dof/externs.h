//  9dof/externs.h
//
//  global variable external definitions for c9dof sensor server
//

#ifndef _9DOF_EXTERNS_H_
#define _9DOF_EXTERNS_H_  1

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

/// serial port values
extern  int     SerialFd;
extern  char    *SerialPort;
extern  int     SerialSpeed;

/// sensor ID and SubID values
extern  char    *SensorId, *SensorSubId;

#endif  /* 9dof/externs.h */
