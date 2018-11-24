//	vision.c
//
//	main driver for vision communications

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "cv_externs.h"



static int  camerasLatest[MAX_CAMERAS] = {0, 0};    // index to latest recvd, wraps as needed


void        process(int);





int
main(int argc, char **argv)

{
	MyName = argv[0];

	cvProcInit(argc, argv);

	while (1) {

		process(HostInfo.sock);
	}

}




void
process(int sock)

{
	char	        buffer[MAX_CAMERA_READ];
	int 	        readRet, cam, scanRet;
    struct timeval  tv;

    if ((readRet = recvfrom(sock, buffer, MAX_CAMERA_READ, 0, (struct sockaddr *)NULL, 0)) > 0) {

        if (readRet < MIN_CAM_REC_SZ) {
            fprintf(stderr, "%s: warning: undersize record recevied (%d bytes)\n", MyName, readRet);
            return;
        }

        buffer[readRet] = '\0';

        if (DebugLevel == DEBUG_DETAIL) {
            printf("Message:\t\"%s\"\n", buffer);
            printf("Next (L / R):\t%d / %d\n", camerasLatest[CAM_LEFT], camerasLatest[CAM_RIGHT]);
        }
    }

    // which camera
    switch (*buffer) {

        case 'L':
            cam = CAM_LEFT;
            break;

        case 'R':
            cam = CAM_RIGHT;
            break;

        default:
            fprintf(stderr, "%s: error: unknown camera '%c'\n", MyName, *buffer);
            return;
    }

    if (*(buffer+1) != ' ') {
        fprintf(stderr, "%s: error: non-space 2nd char '%c'\n", MyName, *(buffer + 1));
        return;
    }

    // set arrival time of message
    gettimeofday(&tv, (struct timezone *)NULL);
    Cameras[cam][camerasLatest[cam]].secs  = tv.tv_sec;
    Cameras[cam][camerasLatest[cam]].usecs = tv.tv_usec;

    if (DebugLevel == DEBUG_DETAIL) {
        printf("Timestamp:\t%ld.%ld\n", tv.tv_sec, tv.tv_usec);
    }

    // start scan after first 2 chars (camera id and space)
    scanRet = sscanf(buffer+2, "%s %s %d %d %d %d", Cameras[cam][camerasLatest[cam]].id,
                     Cameras[cam][camerasLatest[cam]].recType,
                     &Cameras[cam][camerasLatest[cam]].x, &Cameras[cam][camerasLatest[cam]].y,
                     &Cameras[cam][camerasLatest[cam]].w, &Cameras[cam][camerasLatest[cam]].h);

    if (scanRet != 6) {         // scanning error
        if (DebugLevel == DEBUG_DETAIL) {
            printf("scanf() error: ret %d\n", scanRet);
        }
        return;
    }

    if (DebugLevel == DEBUG_DETAIL) {
        printf("Cam: '%c' (%d) (%d)\n\tTimestamp:\t%ld.%08ld\n\tRecType:\t\"%s\"\n\tID:\t\t\"%s\"\n\tX, Y:\t\t%d, %d\n\tW, H:\t\t%d, %d\n\n",
               *buffer, cam, camerasLatest[cam],
               Cameras[cam][camerasLatest[cam]].secs, Cameras[cam][camerasLatest[cam]].usecs,
               Cameras[cam][camerasLatest[cam]].recType, Cameras[cam][camerasLatest[cam]].id,
               Cameras[cam][camerasLatest[cam]].x, Cameras[cam][camerasLatest[cam]].y,
               Cameras[cam][camerasLatest[cam]].w, Cameras[cam][camerasLatest[cam]].x);
    }

    if (++camerasLatest[cam] >= MAX_CAMERA_RECORDS) {
        camerasLatest[cam] = 0;
    }
}
