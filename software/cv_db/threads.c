//  threads.c
//
//  thread handling for cv_proc

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
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

static void *recvCamDataThread(void *);
pthread_t   tidRecieve;

static void
processCamData(int);






void
startPruneThread(int ttl)
{
    pthread_attr_t  attr;
    static  int ttlStatic;      // needs to stay around even after this
                                // function disappears so a pointer to
                                // somethign in the stak frame won't work
                                // unless the thread *IMMEDIATELY* gets
                                // the value before this function returns

    ttlStatic = ttl;

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): starting pruneThread thread...\n", __func__, ttl);
    }

    pthread_attr_init(&attr);

    pthread_create(&tidPrune, &attr, pruneThread, &ttlStatic);

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): pruneThread thread started.\n", __func__, ttl);
    }
}


static void *
pruneThread(void *ttl)
{
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): Starting...\n", __func__, *(int *)ttl);
    }

    while (1) {
        sleep (*(int *)ttl);
#ifdef  DEBUG
        struct timeval  tv;

        gettimeofday(&tv, (struct timezone *)NULL);

        if (DebugLevel == DEBUG_DETAIL) {
#ifdef	__APPLE__
            fprintf(DebugFP, "%s(%d): awake at %ld.%d\n",
#else
            fprintf(DebugFP, "%s(%d): awake at %ld.%ld\n",
#endif
                    __func__, *(int *)ttl, tv.tv_sec, tv.tv_usec);
        }
#endif  // DEBUG
        
        camRecPruneAll(*(int *)ttl);
    }
}





void
startCamDataThread(int sock)
{
    pthread_attr_t  attr;
    static int      sockStatic;

    sockStatic = sock;

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): starting recvCamDataThread thread...\n", __func__, sock);
    }

    pthread_attr_init(&attr);
fprintf(DebugFP, "%s(%d) &sock (0x%lx -> [%d])\n", __func__, sock, (long)&sock, *((int *)&sock));

    pthread_create(&tidPrune, &attr, recvCamDataThread, &sockStatic);

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): recvCamDataThread thread started.\n", __func__, sock);
    }
}




static void *
recvCamDataThread(void *sock)
{
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): Starting...\n", __func__, *(int *)sock);
    }

    while (1) {
        processCamData(*(int *)sock);      // each call processes one record
    }

}


int MsgNum = 0;

static void
processCamData(int sock)

{
	char    buffer[MAX_CAMERA_READ], id[MAX_CAMERA_READ], camera;
	int     readRet, scanRet, x, y, w, h;

    if ((readRet = recvfrom(sock, buffer, MAX_CAMERA_READ, 0, (struct sockaddr *)NULL, 0)) > 0) {

        if (readRet < MIN_CAM_REC_SIZE) {
            if(DebugLevel == DEBUG_DETAIL) {
                fprintf(DebugFP, "%s: warning: undersize record received (%d bytes)\n",
                        MyName, readRet);
            }
            return;
        }

        buffer[readRet] = '\0';

        if (DebugLevel == DEBUG_DETAIL) {
            fprintf(DebugFP, "Message[%d]:\t\"%s\" (len %ld)\n",
                    MsgNum,  buffer, strlen(buffer));
        }
    }

    MsgNum++;

    // validate and set camera
    // should be "[RL] N2 " at head
    if (*buffer == CAMERA_LEFT_ID || *buffer == CAMERA_RIGHT_ID) {
        camera = *buffer;
    } else {
        fprintf(DebugFP, "%s: error: unknown camera '%c'\n", MyName, *buffer);
        return;
    }

    if (*(buffer+1) != ' ') {
        fprintf(DebugFP, "%s: error: non-space 2nd char '%c'\n", MyName, *(buffer + 1));
        return;
    }

    if(*(buffer+2) != 'N' && *(buffer+3) != '2' && *(buffer+4) != ' ') {
        fprintf(DebugFP, "%s: error: invalid record type \'%c%c\'\n",
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

    if (DebugLevel == DEBUG_INFO) {
        fprintf(DebugFP, "%s(%d): recvd and adding msg #%d\n", __func__, sock, MsgNum);
    }
    camRecAdd(id, camera, x, y, w, h);
}
