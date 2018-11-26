//	cv_proc.c
//
//	main driver for vision communications

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "cv_lists.h"
#include "cv_externs.h"



void    process(int);

void    init(int, char **);





int
main(int argc, char **argv)

{
	MyName = argv[0];

    DebugFP = stderr;           // unless overridden on the command line

	init(argc, argv);

	while (1) {

		process(HostInfo.sock);

        camRecPruneById("thing", Ttl);

        int         ret;
        CAMERA_RECORD  results[2];

        ret = camRecGetAvg("thing", results);

        fprintf(DebugFP, "%d = camRecGetAvg(\"thing\", array) results)\n", ret);

        dumpCamRecord(&results[0]);
        dumpCamRecord(&results[1]);
        dumpLists();
	}

}




void
process(int sock)

{
	char    buffer[MAX_CAMERA_READ], id[MAX_CAMERA_READ], camera;
	int     readRet, scanRet, x, y, w, h;

    if ((readRet = recvfrom(sock, buffer, MAX_CAMERA_READ, 0, (struct sockaddr *)NULL, 0)) > 0) {

        if (readRet < MIN_CAM_REC_SIZE) {
            fprintf(stderr, "%s: warning: undersize record recevied (%d bytes)\n", MyName, readRet);
            return;
        }

        buffer[readRet] = '\0';

        if (DebugLevel == DEBUG_DETAIL) {
            fprintf(DebugFP, "Message:\t\"%s\"\n", buffer);
        }
    }

    // validate and set camera
    // should be "[RL] N2 " at head
    if (*buffer == CAMERA_LEFT_ID || *buffer == CAMERA_RIGHT_ID) {
        camera = *buffer;
    } else {
        fprintf(stderr, "%s: error: unknown camera '%c'\n", MyName, *buffer);
        return;
    }

    if (*(buffer+1) != ' ') {
        fprintf(stderr, "%s: error: non-space 2nd char '%c'\n", MyName, *(buffer + 1));
        return;
    }

    if(*(buffer+2) != 'N' && *(buffer+3) != '2' && *(buffer+4) != ' ') {
        fprintf(stderr, "%s: error: invalid record type \'%c%c\'\n",
                MyName, *(buffer+2), *(buffer+3));
        return;
    }

    // seems OK start scan after first 2 chars (camera, space, type, space)
    scanRet = sscanf(buffer+5, "%s %d %d %d %d",
                     id, &x, &y, &w, &h);

    if (scanRet != 5) {         // scanning error
        if (DebugLevel == DEBUG_DETAIL) {
            fprintf(DebugFP, "scanf() error: ret %d\n", scanRet);
        }
        return;
    }

    camRecAdd(id, camera, x, y, w, h);
}
