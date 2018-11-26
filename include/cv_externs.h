//  cv_externs.h
//
//  global variable external definitions for cyborg_vision
//


/// program name (set directly from argv[0])
extern  char        *MyName;

/// network connection for incoming camera messages
extern  HOST_INFO   HostInfo;

/// time-to-live for camera messages
extern  int         Ttl;

/// \brief Global debug level
///
/// 0 - OFF
/// 1 - INFO
/// 2 - DETAILED
extern  int         DebugLevel;
/// Global FP for Debug output
extern  FILE        *DebugFP;


