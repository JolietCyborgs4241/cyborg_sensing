//  threads.c
//
//  thread handling for cv_proc

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "db/lists.h"
#include "db/externs.h"
#include "status/status.h"
#include "sensors.h"


//  four threads:
//
//  main thread - startup, init, and heartbeat to status server
//
//  prune thread - prune sensor records beyond the specified ttl (leaves hdr)
//
//  receive thread - receive sensor records and store them in db
//
//  query thread - receive sensor queries and return them to caller



static void *pruneThread();
pthread_t   tidPrune;

static void *recvSensorPostThread(void *);
pthread_t   tidReceive;

static void *recvSensorQueryThread(void *);
pthread_t   tidQuery;


void processSensorData(int), processSensorQuery(int);




void
startPruneThread()
{
    static pthread_attr_t  attr;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): starting pruneThread thread...\n", __func__);
    }

    pthread_attr_init(&attr);

    pthread_create(&tidPrune, &attr, pruneThread, NULL);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): pruneThread thread started.\n", __func__);
    }
}


static void *
pruneThread()
{
    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): Starting...\n", __func__);
    }

    while (1) {
        usleep (PRUNE_FREQUENCY * 1000);
        
        sensorRecPruneAll();
    }
}



void
startSensorPostThread(int sock)
{
    static pthread_attr_t  attr;
    static int      sockStatic;

    sockStatic = sock;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): starting recvSensorPostThread thread...\n", __func__, sock);
    }

    pthread_attr_init(&attr);

    pthread_create(&tidReceive, &attr, recvSensorPostThread, &sockStatic);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): recvSensorPostThread thread started.\n", __func__, sock);
    }
}




static void *
recvSensorPostThread(void *sock)
{
    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): Starting...\n", __func__, *(int *)sock);
    }

    while (1) {
        processSensorData(*(int *)sock);      // each call processes one record
        sendStatusUpdate(STAT_LED_DB_ACTIVE, COLOR_GREEN);
    }

}



void
startSensorQueryThread(int sock)
{
    static pthread_attr_t  attr;
    static int      sockStatic;

    sockStatic = sock;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): starting recvSensorQueryThread thread...\n", __func__, sock);
    }

    pthread_attr_init(&attr);

    pthread_create(&tidQuery, &attr, recvSensorQueryThread, &sockStatic);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): recvSensorQueryThread thread started.\n", __func__, sock);
    }
}




static void *
recvSensorQueryThread(void *sock)
{
    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): Starting...\n", __func__, *(int *)sock);
    }

    while (1) {
        processSensorQuery(*(int *)sock);      // each call processes one query
        sendStatusUpdate(STAT_LED_DB_ACTIVE, COLOR_BLUE);
        sleep(1);                // for now just spin
    }

}
