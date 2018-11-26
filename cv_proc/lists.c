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




/// \brief  Get a CAM_LIST_HDR for a specific object id
///
/// id
CAM_LIST_HDR *
camListGetHdr(char  *id)
{
    CAM_LIST_HDR    *hdrPtr;

    // any ids at all?
    if ( ! camLists) {
        return NULL;
    }

    hdrPtr = camLists;

    while (hdrPtr) {
        if (strcmp(id, hdrPtr->id) == 0) {
            break;
        }

        hdrPtr = hdrPtr->next;
    }

    return hdrPtr;
}





/// \brief  Add a new camera record
///
/// id, camera, x, y, w, h
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
    hdrPtr = camListGetHdr(id);

    if ( ! hdrPtr) {    // went to the end so add a new header for this id
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

    if ( hdrPtr->recs ) {       // there already is a set of records for this id
        newCamPtr->next = hdrPtr->recs;
        hdrPtr->recs = newCamPtr;
    } else {                    // this is the first one so terminate it
        newCamPtr->next = (CAM_RECORD *)NULL;
        hdrPtr->recs = newCamPtr;
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





/// \brief get the latest right / left camera records for a specific id
///
/// Returns the latest right and left camera records for the specified id
/// via a 2-element array
///
/// returns the number of records found
///
/// -1 - object id not found
///
/// 0 - no records found (return array zeroed)
///
/// 1 - 1 record found (missing record returns 0s for all fields
///
/// 2 - 2 records found
int
camRecGetLatest(char *id, CAM_RECORD *rlCamArray)
{
    CAM_LIST_HDR    *hdrPtr;
    CAM_RECORD      *camPtr;
    int             foundLeft, foundRight;

    if (DebugLevel >= DEBUG_INFO) {
        fprintf(DebugFP, "camRecGetLatest(\"%s\", 0x%lx[])\n",
                id, (long)rlCamArray);
    }

    // first record for any ids?
    if ( ! camLists) {
        return -1;       // no records of any kind
    }

    foundLeft  = 0;
    foundRight = 0;

    hdrPtr     = camListGetHdr(id);

    if ( ! hdrPtr) {
        return -1;      // that id not found
    }

    zeroCamRecord(&rlCamArray[0]);
    zeroCamRecord(&rlCamArray[1]);

    if ( ! hdrPtr->recs) {          // but no camera records
        if (DebugLevel == DEBUG_DETAIL) {
            fprintf(DebugFP, "camRecGetLatest(): no camera records.\n");
        }
        return 0;
    }

    camPtr = hdrPtr->recs;

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "camRecGetLatest(): searching recs starting at (0x%lx)\n",
                (long)camPtr);
    }

    while (camPtr && (! foundLeft || ! foundRight)) {

        if (camPtr->camera == CAMERA_LEFT) {
            if ( ! foundLeft) {     // got a 'left' now
                rlCamArray[CAM_LEFT_OFF] = *camPtr;
                foundLeft = 1;
            }
        }

        if (camPtr->camera == CAMERA_RIGHT) {
            if ( ! foundRight) {     // got a 'right' now
                rlCamArray[CAM_RIGHT_OFF] = *camPtr;
                foundRight = 1;
            }
        }

        camPtr = camPtr->next;
    }

    return (foundLeft + foundRight);
}    






/// \brief get average x, y, w, h, values for a specific id (both cameras)
///
/// Returns the average right and left camera records for the specified id
/// via a 2-element array
///
/// Averages x, y, w, and h values for each camera
///
/// returns the number of records found
///
/// -1 - object id not found
///
/// 0 - no records found (return array zeroed)
///
/// 1 - 1 record found (missing record returns 0s for all fields
///
/// 2 - 2 records found
int
camRecGetAvg(char *id, CAM_RECORD *rlCamArray)
{
    CAM_LIST_HDR    *hdrPtr;
    CAM_RECORD      *camPtr;
    int             rCount, lCount, foundLeft, foundRight;

    if (DebugLevel >= DEBUG_INFO) {
        fprintf(DebugFP, "camRecGetAvg(\"%s\", 0x%lx[])\n",
                id, (long)rlCamArray);
    }

    rCount    = lCount     = 0;
    foundLeft = foundRight = 0;

    // first record for any ids?
    if ( ! camLists) {
        return -1;       // no records of any kind
    }

    hdrPtr = camListGetHdr(id);

    if ( ! hdrPtr) {
        return -1;       // obect id not found
    }

    zeroCamRecord(&rlCamArray[CAM_LEFT_OFF]);
    zeroCamRecord(&rlCamArray[CAM_RIGHT_OFF]);

    rlCamArray[CAM_LEFT_OFF].camera =  'L';
    rlCamArray[CAM_RIGHT_OFF].camera = 'R';

    if ( ! hdrPtr->recs) {          // but no camera records
        if (DebugLevel == DEBUG_DETAIL) {
            fprintf(DebugFP, "camRecGetAvg(): no camera records.\n");
        }
        return 0;
    }

    camPtr = hdrPtr->recs;

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "camRecGetAvg(): searching recs starting at (0x%lx)\n",
                (long)camPtr);
    }

    while (camPtr) {

        if (camPtr->camera == CAMERA_LEFT) {
            rlCamArray[CAM_LEFT_OFF].x += camPtr->x;
            rlCamArray[CAM_LEFT_OFF].y += camPtr->y;
            rlCamArray[CAM_LEFT_OFF].w += camPtr->w;
            rlCamArray[CAM_LEFT_OFF].h += camPtr->h;

            lCount++;
            foundLeft = 1;
        }

        if (camPtr->camera == CAMERA_RIGHT) {
            rlCamArray[CAM_RIGHT_OFF].x += camPtr->x;
            rlCamArray[CAM_RIGHT_OFF].y += camPtr->y;
            rlCamArray[CAM_RIGHT_OFF].w += camPtr->w;
            rlCamArray[CAM_RIGHT_OFF].h += camPtr->h;

            rCount++;
            foundRight = 1;
        }

        camPtr = camPtr->next;
    }

    // been through the whole list do calculate the averages for each
    if (foundLeft) {
        rlCamArray[CAM_LEFT_OFF].x  /= lCount;
        rlCamArray[CAM_LEFT_OFF].y  /= lCount;
        rlCamArray[CAM_LEFT_OFF].w  /= lCount;
        rlCamArray[CAM_LEFT_OFF].h  /= lCount;
    }

    if (foundRight) {
        rlCamArray[CAM_RIGHT_OFF].x /= rCount;
        rlCamArray[CAM_RIGHT_OFF].y /= rCount;
        rlCamArray[CAM_RIGHT_OFF].w /= rCount;
        rlCamArray[CAM_RIGHT_OFF].h /= rCount;
    }

    return (foundLeft + foundRight);
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
        if (DebugLevel == DEBUG_DETAIL) {
            dumpCamListHdr(camListHdr);
        }

        CAM_RECORD  *camRecs = camListHdr->recs;

        while (camRecs) {
            if (DebugLevel == DEBUG_DETAIL) {
                dumpCamRecord(camRecs);
            }

            camRecs = camRecs->next;
        }
        camListHdr = camListHdr->next;
    }
    fprintf(DebugFP, "^------------^\n");
}


/// \brief dump an individual CAM_LIST_HDR object to the debug output
///
void
dumpCamListHdr(CAM_LIST_HDR *ptr)
{
    fprintf(DebugFP, "CAM_LIST_HDR @ (0x%lx):\n", (long)ptr);

    fprintf(DebugFP, "\tID:\t\"%s\"\n\tRecs:\t(0x%lx\n\tNext:\t(0x%lx)\n",
            ptr->id, (long)ptr->recs, (long)ptr->next);
}



/// \brief dump an individual CAM_RECORD object to the debug output
///
void
dumpCamRecord(CAM_RECORD *ptr)
{
    fprintf(DebugFP, "CAM_RECORD @ (0x%lx):\n", (long)ptr);

    fprintf(DebugFP, "\tTime:\t%ld.%08ld\n\tCamera:\t%c\n\tx, y:\t%d, %d\n\tw, h:\t%d, %d\n\tNext:\t(0x%lx)\n",
            ptr->secs, ptr->usecs, ptr->camera,
            ptr->x, ptr->y, ptr->w, ptr->h,
            (long)ptr->next);
}




/// \brief zero all fields in an individual CAM_RECORD object to the debug output
///
void
zeroCamRecord(CAM_RECORD *ptr)
{
    ptr->secs = ptr->usecs = 0;
    ptr->camera = ' ';
    ptr->x = ptr->y = ptr->w = ptr->h = 0;
    ptr->next = (CAM_RECORD *)NULL;
}
