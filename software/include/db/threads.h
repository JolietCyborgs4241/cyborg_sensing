//  db/threads.h
//
//  database thread-related stuff

#ifndef _CV_DB_THREADS_H_
#define _CV_DB_THREADS_H_ 1


/// Start the purging thread
void    startPruneThread();

/// start the receiving thread for posting sensor data
///
/// socket fd
void    startSensorPostThread(int);

/// start the receiving thread for querying sensor data
///
/// socket fd
void    startSensorQueryThread(int);


#endif  /* db/threads.h */
