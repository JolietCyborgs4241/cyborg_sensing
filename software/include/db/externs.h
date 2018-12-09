//  db/externs.h
//
//  global variable external definitions for cyborg_vision database
//

#ifndef _DB_EXTERNS_H_
#define _DB_EXTERNS_H_  1

/// program name (set directly from argv[0])
extern  char        *MyName;

/// network connection for incoming camera messages
extern  HOST_INFO   HostInfo;

extern  int         Ttl;

/// \brief Global debug level
///
/// 0 - OFF
/// 1 - INFO
/// 2 - DETAILED
extern  int         DebugLevel;
/// Global FP for Debug output
extern  FILE        *DebugFP;

/// time-to-live for sensor data
///
/// could potentially be distinct for each sensor type
extern  TTLS    SensorTtls[];


void    initMutexes();
void    initDb();

#endif  /* db/externs.h */
