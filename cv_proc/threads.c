//  threads.c
//
//  thread handling for cv_proc

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "cv_lists.h"
#include "cv_externs.h"


//  three threads:
//
//  main thread - respond to requests and calculate direction and distances
//
//  prune thread - prune camera records beyond the specified ttl (leaves hdr)
//
//  receive thread - receive camera records and store them for other threads



static void *pruneThread(void *);
pthread_t   tidPrune;

static void *receiveThread(void *);
pthread_t   tidRecieve;


void
startPruneThread(int ttl)
{
    pthread_attr_t  attr;

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): starting prune(%d) thread...\n", __func__, ttl);
    }

    pthread_attr_init(&attr);

    pthread_create(&tidPrune, &attr, pruneThread, &ttl);

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): prune(%d) thread started.\n", __func__, ttl);
    }
}


static void *
pruneThread(void *ttl)
{
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): Starting...\n", __func__, *((int *)ttl));
    }

    while (1) {
        sleep (*((int *)ttl));
#ifdef  DEBUG
        struct timeval  tv;

        gettimeofday(&tv, (struct timezone *)NULL);

        if (DebugLevel == DEBUG_DETAIL) {
            fprintf(DebugFP, "%s(%d): awake at %ld.%ld\n",
                    __func__, *((int *)ttl), tv.tv_sec, tv.tv_usec);
        }
#endif  // DEBUG
        
        camRecPruneAll(*((int *)ttl));
    }
}





void
startReceiveThread(int sock)
{
    pthread_attr_t  attr;

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): starting receive(%d) thread...\n", __func__, sock);
    }

    pthread_attr_init(&attr);

    pthread_create(&tidPrune, &attr, receiveThread, &sock);

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): recieve(%d) thread started.\n", __func__, sock);
    }
}




void *
receiveThread(void *param)
{

}
