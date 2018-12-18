//	9dof_server.c
//
//	server to read and post sensor readings to sensor database

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
#include "sensors.h"
#include "9dof/externs.h"


#define SCALE_FACTOR    100     // multiple the readings by this so
                                // we can deal with them as integers

#define BUFFER_SIZE     200

char    OutAccellBuffer[BUFFER_SIZE];
char    OutRollBuffer[BUFFER_SIZE];
char    OutMagBuffer[BUFFER_SIZE];

char    *PtrAccellBuffer, *PtrRollBuffer, *PtrMagBuffer;

void    init(int, char **), process9dofRecord(char *),
        setAndSend(char *, char *, int, int, int);



int
main(int argc, char **argv)

{
    char            inBuffer[BUFFER_SIZE];
    int             readRet;
    struct timeval  now;

	MyName = argv[0];

    DebugFP = stderr;           // unless overridden on the command line

	init(argc, argv);
    //
    // the 9dof sends all readings together
    //
    // we make it look like we're 3 different sensors (G, O, and M types)
    //
    // initialize the start of the buffer for each sensor "type"
    // to save time later
    sprintf(OutAccellBuffer, "%c %s %s ", SENSOR_ACCELL, SensorId, SensorSubId);
    sprintf(OutRollBuffer, "%c %s %s ", SENSOR_ROLL, SensorId, SensorSubId);
    sprintf(OutMagBuffer, "%c %s %s ", SENSOR_MAGNETIC, SensorId, SensorSubId);

    // these pointers point to where the variant data will start, again,
    // to save time later
    PtrAccellBuffer = OutAccellBuffer + strlen (OutAccellBuffer);
    PtrRollBuffer   = OutRollBuffer + strlen (OutRollBuffer);
    PtrMagBuffer    = OutMagBuffer + strlen (OutMagBuffer);

	while ((readRet = read(SerialFd, inBuffer, sizeof(inBuffer))) != -1) {

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

        process9dofRecord(inBuffer);
	}

    exit(1);    // error on read
}




void
process9dofRecord(char *record)
{
    int             sscanfRet;
    int             sensorTime;
    int             intAccX, intAccY, intAccZ,
                    intRollX, intRollY, intRollZ,
                    intMagX, intMagY, intMagZ;
    float           accX, accY, accZ,
                    rollX, rollY, rollZ,
                    magX, magY, magZ;
    struct timeval  now;

    sscanfRet = sscanf(record, "%d, %f, %f, %f, %f, %f, %f, %f, %f, %f",
                       &sensorTime,
                       &accX,  &accY,  &accZ,
                       &rollX, &rollY, &rollZ,
                       &magX,  &magY,  &magZ);

    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s(\"%s\"): sscanfRet = %d\n",
                __func__, record, sscanfRet);
    }

    if (sscanfRet != 10) {
        return;       // something messed up
    }

    // since the rest of the system deal with integers, we'll scale
    // these up 100X before sending and move then to integers
    intAccX  = accX  * SCALE_FACTOR;
    intAccY  = accY  * SCALE_FACTOR;
    intAccZ  = accZ  * SCALE_FACTOR;
    intRollX = rollX * SCALE_FACTOR;
    intRollY = rollY * SCALE_FACTOR;
    intRollZ = rollZ * SCALE_FACTOR;
    intMagX  = magX  * SCALE_FACTOR;
    intMagY  = magY  * SCALE_FACTOR;
    intMagZ  = magZ  * SCALE_FACTOR;

    // append the actual sensor values for this read to the already
    // set records for each logical sensor type
    setAndSend(OutAccellBuffer, PtrAccellBuffer, intAccX,  intAccY,  intAccZ);
    setAndSend(OutRollBuffer,   PtrRollBuffer,   intRollX, intRollY, intRollZ);
    setAndSend(OutMagBuffer,    PtrMagBuffer,    intMagX,  intMagY,  intMagZ);
}



void
setAndSend(char *fullBuffer, char *start, int x, int y, int z)
{
    ssize_t         retVal;
    struct timeval  now;

    sprintf(start, "%d %d %d", x, y, z);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): buffer to send \"%s\"\n",
                __func__, fullBuffer);
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
                LogID, LOG_DIR_OUT, fullBuffer);
    }

    if ((retVal = sendto(HostInfo.sock, fullBuffer, strlen(fullBuffer), 0,
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
