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
#include "sensors.h"


//  three threads:
//
//  main thread - respond to requests and calculate direction and distances
//
//  prune thread - prune camera records beyond the specified ttl (leaves hdr)
//
//  receive thread - receive camera records and store them for other threads



static void *pruneThread();
pthread_t   tidPrune;

static void *recvSensorDataThread(void *);
pthread_t   tidRecieve;

static void processSensorData(int sock);

void processCamData(char *);    // all in sensors.c
void processRangerData(char *);
void process9DData(char *);     // all G, roll, and mag look the same




void
startPruneThread()
{
    pthread_attr_t  attr;

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
#ifdef  DEBUG
        struct timeval  tv;

        gettimeofday(&tv, NULL);

        if (DebugLevel >= DEBUG_DETAIL) {
#ifdef	__APPLE__
            fprintf(DebugFP, "%s(): awake at %ld.%d\n",
#else
            fprintf(DebugFP, "%s(): awake at %ld.%ld\n",
#endif
                    __func__, tv.tv_sec, tv.tv_usec);
        }
#endif  // DEBUG
        
        sensorRecPruneAll();
    }
}





void
startSensorDataThread(int sock)
{
    pthread_attr_t  attr;
    static int      sockStatic;

    sockStatic = sock;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): starting recvSensorDataThread thread...\n", __func__, sock);
    }

    pthread_attr_init(&attr);
fprintf(DebugFP, "%s(%d) &sock (0x%lx -> [%d])\n", __func__, sock, (long)&sock, *((int *)&sock));

    pthread_create(&tidPrune, &attr, recvSensorDataThread, &sockStatic);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): recvSensorDataThread thread started.\n", __func__, sock);
    }
}




static void *
recvSensorDataThread(void *sock)
{
    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): Starting...\n", __func__, *(int *)sock);
    }

    while (1) {
        processSensorData(*(int *)sock);      // each call processes one record
    }

}


int MsgNum = 0;

static void
processSensorData(int sock)
{
        char    buffer[MAX_SENSOR_READ];
        int     readRet;

    if ((readRet = recvfrom(sock, buffer, MAX_SENSOR_READ, 0, (struct sockaddr *)NULL, 0)) > 0) {

        buffer[readRet] = '\0';

        if (DebugLevel >= DEBUG_DETAIL) {
            fprintf(DebugFP, "Message[%d]:\t\"%s\" (len %ld)\n",
                    MsgNum,  buffer, strlen(buffer));
        }

        switch (*buffer) {               // first character identifies sensor type

        case SENSOR_CAMERA:
            processCamData(buffer);
            break;

        case SENSOR_RANGE:
            processRangerData(buffer);
            break;

        case SENSOR_ACCELL:
        case SENSOR_ROLL:
        case SENSOR_MAGNETIC:
            process9DData(buffer);
            break;

        default:
            fprintf(DebugFP, "%s: %s(): invalid sensor type '%c'\n", MyName, __func__, *buffer);
        }

        MsgNum++;

    } else {
        fprintf(DebugFP, "%s(%d): read error \"%s\"\n", __func__, sock, strerror(errno));
    }
}
