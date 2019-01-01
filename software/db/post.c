//  post.c
//
//  handle incoming sensor updates and post to db

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


int MsgNum = 0;

void processCamData(char *);    // all in sensors.c
void processOneValSensor(char *);
void process9DData(char *);     // all G, roll, and mag look the same



void
processSensorData(int sock)
{
    char    buffer[MAX_SENSOR_READ];
    int     readRet, msgRate;
    struct  timeval now, timeDiff;
    float   floatTime;

    if ((readRet = recvfrom(sock, buffer, MAX_SENSOR_READ, 0, (struct sockaddr *)NULL, 0)) > 0) {

        gettimeofday(&now, NULL);

        buffer[readRet] = '\0';

        if (LogFP) { 
#ifdef  __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06d %s %s \"%s\"\n",
#else   // ! __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06ld %s %s \"%s\"\n",
#endif
                    MyName, now.tv_sec, now.tv_usec,
                    LogID, LOG_DIR_IN, buffer);
        }

        if (DebugLevel >= DEBUG_INFO) {
            fprintf(DebugFP, "Message[%d]:\t\"%s\" (len %ld)\n",
                    MsgNum,  buffer, strlen(buffer));
            if ( MsgNum && (MsgNum % MsgRateReportingCadence) == 0) {

                timersub(&now, &StartTime, &timeDiff);

                // convert the time difference to a decimal number of seconds
                // we convert the integer microsecond field into a fraction of a second
                floatTime = (float)timeDiff.tv_sec + ((float)timeDiff.tv_usec / 1000000.0);
                msgRate = (float)MsgNum / floatTime;

                fprintf(DebugFP, "<<<< Message rate @ %d msgs per second >>>>\n", msgRate);
            }
        }

        switch (*buffer) {               // first character identifies sensor type

        case SENSOR_CAMERA:
            processCamData(buffer);
            break;

        case SENSOR_RANGE:
        case SENSOR_OBSTACLE:
            processOneValSensor(buffer);
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
