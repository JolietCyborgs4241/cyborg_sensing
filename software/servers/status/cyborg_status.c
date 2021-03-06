//	gen_sss.c
//
//	generic serial sensor server to read and post sensor
//	readings to sensor database
//
//  supports sensors that provide db-ready output readings
//
//  creates one thread per serial connection

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "status/externs.h"
#include "status/status.h"



void        init(int, char **);

static void processStatusUpdate(int sock, int serial);


int
main(int argc, char **argv)

{
	MyName = argv[0];

	DebugFP = stderr;
    
    init(argc, argv);

    gettimeofday(&StartTime, NULL);

	while (1) {
        processStatusUpdate(HostInfo.sock, SerialFd);
    }
}



int MsgNum = 0;

static void
processStatusUpdate(int sock, int serial)
{
    char    cmd, buffer[MAX_STATUS_LEN];
    int     readRet, writeRet, msgRate;
    struct  timeval now, timeDiff;
    float   floatTime;

    if ((readRet = recvfrom(sock, buffer, MAX_STATUS_LEN, 0, (struct sockaddr *)NULL, 0)) > 0) {

        if (readRet != sizeof(STATUS_MSG)) {
            fprintf(DebugFP, "%s(%d, %d): read error - returned %d bytes - \"%s\"\n",
                    __func__, sock, serial, readRet, strerror(errno));
            return;             // not sure what this is
        }

        gettimeofday(&now, NULL);

        if (LogFP) {
#ifdef  __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06d %s %s (0x%02x%02x) (len = %d)\n",
#else   // ! __APPLE__
            fprintf(LogFP, "%s LOG %ld.%06ld %s %s (0x%02x%02x) (len = %d)\n",
#endif
                    MyName, now.tv_sec, now.tv_usec,
                    LogID, LOG_DIR_IN, (unsigned char)*buffer, (unsigned char)*(buffer+1),
                    readRet);
        }

        if (DebugLevel >= DEBUG_INFO) {
            fprintf(DebugFP, "Message[%d]:\t(0x%02x%02x)\n",
                    MsgNum,  (unsigned char)*buffer, (unsigned char) *(buffer+1));
            if ( MsgNum && (MsgNum % MsgRateReportingCadence) == 0) {

                timersub(&now, &StartTime, &timeDiff);

                // convert the time difference to a decimal number of seconds
                // we convert the integer microsecond field into a fraction of a second
                floatTime = (float)timeDiff.tv_sec + ((float)timeDiff.tv_usec / 1000000.0);
                msgRate = (float)MsgNum / floatTime;

                fprintf(DebugFP, "<<<< Message rate @ %d msgs per second >>>>\n", msgRate);
            }
        }

        MsgNum++;

        cmd = *buffer | *(buffer + 1);

        if (DebugLevel >= DEBUG_INFO) {
            fprintf(DebugFP, "%s(): sending to serial port (0x%02x)\n", __func__, cmd);
        }

        // off to the status display hardware with the record
        if ((writeRet = write(serial, &cmd, sizeof(cmd))) != sizeof(cmd)) {
            fprintf(DebugFP, "%s(%d, %d): write error - returned %d bytes - \"%s\"\n",
                    __func__, sock, serial, writeRet, strerror(errno));
            exit(1);    // once an error happens, it's usually fatal
        }

    } else {
        fprintf(DebugFP, "%s(%d, %d): read error \"%s\"\n",
                __func__, sock, serial, strerror(errno));
    }
}
