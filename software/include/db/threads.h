//  db/threads.h
//
//  database thread-related stuff

#ifndef _CV_DB_THREADS_H_
#define _CV_DB_THREADS_H_ 1


/// Start the purging thread
///
/// ttl
void    startPruneThread();

/// start the receiving thread
///
/// socket fd
void    startSensorDataThread(int);


#endif  /* db/threads.h */
