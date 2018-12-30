//  db/externs.h
//
//  global variable external definitions for cyborg_vision database
//

#ifndef _DB_EXTERNS_H_
#define _DB_EXTERNS_H_  1

/// program name (set directly from argv[0])
extern  char        *MyName;

/// network connection for incoming sensor messages
extern  HOST_INFO   HostInfoPost;

/// network connection for sensor queries
extern  HOST_INFO   HostInfoQuery;

/// network connection for status server
extern  HOST_INFO   StatusServer;

extern  int         Ttl;

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

/// time-to-live for sensor data
///
/// could potentially be distinct for each sensor type
extern  TTLS    SensorTtls[];

/// Program start time
extern  struct timeval  StartTime;


void    initMutexes();
void    initDb();


int     MsgRateReportingCadence;

#endif  /* db/externs.h */
