//  threads.c
//
//  thread handling for cv_proc

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "cv_lists.h"
#include "cv_externs.h"


//  three threads:
//
//  main thread - respond to requests and calculate direction and distances
//
//  purge thread - purge camera records beyond the specified ttl (leaves hdr)
//
//  receive thread - receive camera records and store them for other threads






void
startThreadPurge(int ttl)
{


}






void
startThreadRecv(int sock)
{


}
