//  query.c
//
//  handle incoming sensor queries

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
#include "db/query.h"
#include "db/externs.h"
#include "status/status.h"
#include "sensors.h"


static  int QueryNum = 0;

static  char    retBuffer[MAX_QUERY_RESP];

void
processSensorQuery(int sock)
{
    char    buffer[MAX_QUERY_SIZE],
            queryTag[MAX_QUERY_SIZE], querySensorId[MAX_QUERY_SIZE],
            querySensorSubId[MAX_QUERY_SIZE],
            queryType, querySensor;
    int     readRet, sendRet, sscanfRet, retSize;
    struct  timeval now;
    struct sockaddr srcAddr;
    socklen_t addrLen = sizeof(srcAddr);

    if ((readRet = recvfrom(sock, buffer, MAX_QUERY_SIZE, 0, &srcAddr, &addrLen)) > 0) {

        if (DebugLevel >= DEBUG_DETAIL) {
            fprintf(DebugFP, "%s(%d): read %d bytes\n", __func__, sock, readRet);
        }
    
        gettimeofday(&now, NULL);

        buffer[readRet] = '\0';

        QueryNum++;

        if (LogFP) {
#ifdef  __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06d %s %s \"%s\"\n",
#else   // ! __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06ld %s %s \"%s\"\n",
#endif
                    MyName, now.tv_sec, now.tv_usec,
                    LogID, LOG_DIR_QUERY, buffer);
        }

        if (DebugLevel >= DEBUG_INFO) {
            fprintf(DebugFP, "Query[%d]:\t\"%s\" (len %ld)\n",
                    QueryNum,  buffer, strlen(buffer));
        }

        // breakdown the query

        if ((sscanfRet = sscanf(buffer, "%s %c %c %s %s",
                                queryTag, &queryType, &querySensor,
                                querySensorId, querySensorSubId)) != 5) {

            if (DebugLevel >= DEBUG_INFO) {
                fprintf(DebugFP, "%s(): sscanf error - returned %d\n",
                        __func__,  sscanfRet);
            }

            return;
        }

        processQuery(queryTag, queryType, querySensor,
                     querySensorId, querySensorSubId,
                     retBuffer, sizeof(retBuffer));

        retSize = strlen(retBuffer);

        if (DebugLevel >= DEBUG_DETAIL) {
            fprintf(DebugFP, "%s():\n\"%s\"\n",
                    __func__, retBuffer);
        }

        if (LogFP) {
#ifdef  __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06d %s %s \"%s\"\n",
#else   // ! __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06ld %s %s \"%s\"\n",
#endif
                    MyName, now.tv_sec, now.tv_usec,
                    LogID, LOG_DIR_RESP, retBuffer);
        }

        if (DebugLevel >= DEBUG_DETAIL) {
            fprintf(DebugFP, "%s(): sending to %s %d\n",
                    __func__, inet_ntoa(((struct sockaddr_in *)&srcAddr)->sin_addr),
                    ((struct sockaddr_in *)&srcAddr)->sin_port);
        }

        if((sendRet = sendto(sock, retBuffer, retSize, 0, &srcAddr, addrLen)) != retSize) {
            if (DebugLevel >= DEBUG_INFO) {
                fprintf(DebugFP, "%s(): sendto() error - returned %d - expected %d (%s)\n",
                        __func__, sendRet, retSize, strerror(errno));
            }
        }
    }
}
