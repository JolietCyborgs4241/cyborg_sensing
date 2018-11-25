//    cv_lists.c
//
//    manipulation functiosn for camera data lists

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "cv_externs.h"
#include "cv_lists.h"


static  CAM_LIST_HDR *camLists = (CAM_LIST_HDR *)NULL;


//  camRecAdd
//
//  
int camRecAdd(char *id, char camera, int x, int y, int w, int h)
{
    CAM_LIST_HDR    *hdrPtr, *newHdrPtr;
    CAM_RECORD      *newCamPtr;
    struct timeval  tv;

    if (DebugLevel == DEBUG_INFO) {
        fprintf(DebugFP, "camRecAdd(\"%s\", \'%c\', %d, %d, %d, %d)\n",
                id, camera, x, y, w, h);
    }

    // first record for any ids?
    if ( ! camLists) {
        newHdrPtr       = (CAM_LIST_HDR *)cvAlloc(sizeof(CAM_LIST_HDR));
        newHdrPtr->id   = (char *)cvAlloc(strlen(id) + 1);
        strcpy(newHdrPtr->id, id);
        newHdrPtr->next = (CAM_LIST_HDR *)NULL;

        camLists = newHdrPtr;
    }

    // search for the id (even if we just added it)
    hdrPtr = camLists;
    while (hdrPtr) {
        if (strcmp(id, hdrPtr->id) == 0)  {    // found one so add it
            break;                          // all done here
        }

        hdrPtr = hdrPtr->next;
    }

    if ( ! hdrPtr) {                        // went to the end so add a new header for this id
        newHdrPtr       = (CAM_LIST_HDR *)cvAlloc(sizeof(CAM_LIST_HDR));
        newHdrPtr->id   = (char *)cvAlloc(strlen(id) + 1);
        strcpy(newHdrPtr->id, id);

        // now add it to the front of the lists which should thread safe
        // since it won't impact anyone already into the processing of
        // the lists (they just won't see the new adition this time through
        newHdrPtr->next = camLists;
        camLists        = newHdrPtr;

        hdrPtr = newHdrPtr;                    // set to make the rest of the code work either way
    }

    // create and populate a camera record
    newCamPtr = (CAM_RECORD *)cvAlloc(sizeof(CAM_RECORD));

    gettimeofday(&tv, (struct timezone *)NULL);     // timestamp it
    newCamPtr->secs   = tv.tv_sec;
    newCamPtr->usecs  = tv.tv_usec;

    newCamPtr->camera = camera;
    newCamPtr->x      = x;
    newCamPtr->y      = y;
    newCamPtr->w      = w;
    newCamPtr->h      = h;

    if ( hdrPtr->recs ) {                       // there already is a set of records for this id
        newCamPtr->next = hdrPtr->recs;
        hdrPtr->recs = newCamPtr;
    } else {                                    // this is the first one so terminate it
        newCamPtr->next = (CAM_RECORD *)NULL;
        hdrPtr->recs = newCamPtr;
    }

    if (DebugLevel == DEBUG_DETAIL) {
        dumpLists();
    }

    return 1;
}




    // ID, TTL - delete old records for an id (all cameras)
int
camRecPruneById(char *id, int ttl)
{

}





     // TTL - prune all camera records older than TTL
int
camRecPrune(int ttl)
{

}





     // ID - delete all records for an id (all cameras)
int
camRecDeleteById(char *id)
{

}





    // get latest record for an id (both cameras)
int
camRecGetLatest(char *id, CAM_RECORD *rlCamArray)
{

}





    // get average x, y, w, h, values for an id (both cameras)
int
camRecGetAvg(char *id, CAM_RECORD *rlCamArray)
{

}



void
dumpLists()
{
    CAM_LIST_HDR    *camListHdr = camLists;

    fprintf(DebugFP, "dumpLists():\nv------------v\n");

    if (! camLists ) {
        fprintf(DebugFP, "\tNO CAMERA RECORDS\n");
        fprintf(DebugFP, "^------------^\n");
        return;
    }

    while (camListHdr) {
        fprintf(DebugFP, "\t(0x%lx) ID: \"%s\"  recs: (0x%lx)  next: (0x%lx)\n",
                (long)camListHdr, camListHdr->id, (long)camListHdr->recs,
                (long)camListHdr->next);

        CAM_RECORD  *camRecs = camListHdr->recs;

        while (camRecs) {
            fprintf(DebugFP, "\t\t(0x%lx) Time: %ld.%08ld  Camera: %c  x,y: %d,%d  w,h: %d, %d  next: (0x%lx)\n",
                    (long)camRecs, camRecs->secs, camRecs->usecs,
                    camRecs->camera,
                    camRecs->x, camRecs->y, camRecs->w, camRecs->h,
                    (long)camRecs->next);

            camRecs = camRecs->next;
        }
        camListHdr = camListHdr->next;
    }
    fprintf(DebugFP, "^------------^\n");
}
