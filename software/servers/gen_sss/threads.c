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
#include "gen_sss/sensors.h"
#include "gen_sss/externs.h"



#define MAX_SENSOR_READ     200


//  two+ threads:
//
//  main thread - idles after initialization and kicking off the new threads
//
//  sensor thread(s) - one per sensor - each opens a serial port, reads data,
//  formats as needed, and posts to the database


static void    startSensorDataThread(SENSOR_CONN *),
               *recvSensorDataThread(void *);




void
KickOffSensorThreads(SENSOR_CONN *sensor)
{
    if (DebugLevel >= DEBUG_INFO) {
        fprintf(DebugFP, "%s() starting...\n", __func__);
    }

    while (sensor) {
        if (DebugLevel >= DEBUG_INFO) {
            fprintf(DebugFP, "%s() starting thread for \"%s\"...\n",
                    __func__, sensor->serialPort);
        }

        startSensorDataThread(sensor);
        sensor = sensor->next;
    }
}




static void
startSensorDataThread(SENSOR_CONN *sensor)
{
    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx): \"%s\" entered...\n",
                __func__, (long)sensor, sensor->serialPort);
    }

    pthread_attr_init(&(sensor->attr));

    pthread_create(&(sensor->tid), &(sensor->attr), &recvSensorDataThread, sensor);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx): recvSensorDataThread thread started.\n",
                __func__, (long)sensor);
    }
}




static void *
recvSensorDataThread(void *sensor)
{
    char            inBuffer[MAX_SENSOR_READ];
    int             readRet, sockRet;
    struct timeval  now;
    SENSOR_CONN     *thisSensor = (SENSOR_CONN *)sensor;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx): Starting...\n", __func__, (long)sensor);
    }

    while ((readRet = read(thisSensor->fd, inBuffer, sizeof(inBuffer))) != -1) {

        inBuffer[readRet] = '\0';

        if (readRet > 2) {          // kill CR/LF @ end
            inBuffer[readRet-2] = '\0';
        }

        if (LogFP) {
            gettimeofday(&now, NULL);

            fprintf(LogFP,
#ifdef  __APPLE__
                    "%s: LOG %ld.%06d %s %s \"%s\"\n",
#else   // ! __APPLE__
                    "%s: LOG %ld.%06ld %s %s \"%s\"\n",
#endif
                    MyName, now.tv_sec, now.tv_usec,
                    LogID, LOG_DIR_IN, inBuffer);
        }

        if ((sockRet = sendto(HostInfo.sock, inBuffer, strlen(inBuffer), 0,
                              (struct sockaddr *)&HostInfo.hostIP,
                              sizeof(struct sockaddr_in))) == -1) {

            fprintf(stderr, "%s: error: %s(): sendto() error (%s)\n",
                    MyName, __func__, strerror(errno));
            exit(1);
        }

        if (DebugLevel >= DEBUG_SUPER) {
            fprintf(DebugFP, "%s():  Sent.\n", __func__);
        }
    }

    if (LogFP) {
        fprintf(LogFP, "%s(): error: read failure from serial port \"%s\" (%s)\n",
                __func__, thisSensor->serialPort, strerror(errno));
    }

    return NULL;
}
