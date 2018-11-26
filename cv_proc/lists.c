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


static  CAMERA_LIST_HDR *camLists = (CAMERA_LIST_HDR *)NULL;

static void freeCamRecsFromEnd(CAMERA_RECORD *); // RECURSIVE!!




/// \brief  Get a CAMERA_LIST_HDR for the list
///
CAMERA_LIST_HDR *
camListGetHdr()
{
    return(camLists);
}




/// \brief  Get a CAMERA_LIST_HDR for a specific object id
///
/// id
CAMERA_LIST_HDR *
camListGetHdrById(char *id)
{
    CAMERA_LIST_HDR    *hdrPtr;

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
int
camRecAdd(char *id, char camera, int x, int y, int w, int h)
{
    CAMERA_LIST_HDR    *hdrPtr, *newHdrPtr;
    CAMERA_RECORD      *newCamPtr;
    int                idx;

    if (DebugLevel == DEBUG_INFO) {
        fprintf(DebugFP, "camRecAdd(\"%s\", \'%c\', %d, %d, %d, %d)\n",
                id, camera, x, y, w, h);
    }

    // first record for any ids?
    if ( ! camLists) {
        newHdrPtr       = (CAMERA_LIST_HDR *)cvAlloc(sizeof(CAMERA_LIST_HDR));

        newHdrPtr->id   = (char *)cvAlloc(strlen(id) + 1);
        strcpy(newHdrPtr->id, id);

        newHdrPtr->recs[CAMERA_LEFT_IDX]  = (CAMERA_RECORD *)NULL;
        newHdrPtr->recs[CAMERA_RIGHT_IDX] = (CAMERA_RECORD *)NULL;
        newHdrPtr->next                   = (CAMERA_LIST_HDR *)NULL;

        camLists = newHdrPtr;
    }

    // search for the id (even if we just added it)
    hdrPtr = camListGetHdrById(id);

    if ( ! hdrPtr) {        // went to the end so add a new header for this id
        newHdrPtr       = (CAMERA_LIST_HDR *)cvAlloc(sizeof(CAMERA_LIST_HDR));

        newHdrPtr->id   = (char *)cvAlloc(strlen(id) + 1);
        strcpy(newHdrPtr->id, id);

        // now add it to the front of the lists which should thread safe
        // since it won't impact anyone already into the processing of
        // the lists (they just won't see the new adition this time through
        newHdrPtr->next = camLists;
        camLists        = newHdrPtr;

        newHdrPtr->recs[CAMERA_LEFT_IDX]  = (CAMERA_RECORD *)NULL;
        newHdrPtr->recs[CAMERA_RIGHT_IDX] = (CAMERA_RECORD *)NULL;

        hdrPtr = newHdrPtr; // set to make the rest of the code work either way
    }

    // create and populate a camera record
    newCamPtr = (CAMERA_RECORD *)cvAlloc(sizeof(CAMERA_RECORD));

    gettimeofday(&(newCamPtr->time), (struct timezone *)NULL); // timestamp it

    newCamPtr->camera = camera;
    newCamPtr->x      = x;
    newCamPtr->y      = y;
    newCamPtr->w      = w;
    newCamPtr->h      = h;

    idx = camera == CAMERA_LEFT_ID ? CAMERA_LEFT_IDX : CAMERA_RIGHT_IDX;

    if ( hdrPtr->recs[idx] ) {  // there already is a set of records for this id
        newCamPtr->next = hdrPtr->recs[idx];
        hdrPtr->recs[idx] = newCamPtr;
    } else {                    // this is the first one so terminate it
        newCamPtr->next = (CAMERA_RECORD *)NULL;
        hdrPtr->recs[idx] = newCamPtr;
    }

    return 1;
}




    // ID, TTL - delete old records for an id (all cameras)
int
camRecPruneById(char *id, int ttl)
{

}





/// \brief  remove any camera records that are beyond the ttl (in secs)
///
/// The list of camera records is ordered with the newest at the front
/// so once we find one that is past the TTL, they are all past the TTL from
/// that point onward
int
camRecPrune(int ttl)
{
    CAMERA_LIST_HDR    *hdrPtr;
    CAMERA_RECORD      *camPtr, *camPtrPrev;;
    struct timeval  tvNow, tvDiff;
    int             i;

    if ((hdrPtr = camListGetHdr()) == (CAMERA_LIST_HDR *)NULL) {
        return 0;             // the whole list is empty
    }
    
    gettimeofday(&tvNow, (struct timezone *)NULL);     // timestamp it

    
    // walk the list of headers
    while (hdrPtr) {
        // walk down the camera recs (if there are any)
        // need to go down the right and the left camera lists
        for (i = 0 ; i < NUM_OF_CAMERAS ; i++) {
            
            camPtr = camPtrPrev = hdrPtr->recs[i];

            while (camPtr) {

                timersub(&tvNow, &(camPtr->time), &tvDiff);

                if (tvDiff.tv_sec >= ttl) {     // too old and so is the rest

                    if (camPtr == hdrPtr->recs[i]) {
 
                        // all of them starting from the newest are old so just
                        // unhook them all
                        hdrPtr->recs[i] = (CAMERA_RECORD *)NULL;

                    } else {

                        // unhook from the previous record
                        camPtrPrev->next = (CAMERA_RECORD *)NULL;
                    }

                    // now do a little recursion to free all of them
                    // from the end

                    freeCamRecsFromEnd(camPtr);
                }

                camPtrPrev = camPtr;
                camPtr = camPtr->next;
            }
        }

        hdrPtr = hdrPtr->next;
    }
    return 1000;
}


static void
freeCamRecsFromEnd(CAMERA_RECORD *ptr)
{

    if (ptr->next) {    // keep going
        freeCamRecsFromEnd(ptr->next);  // RECURSIVE!!!
    } else {
        cvFree(ptr);    // free ME!
    }
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
camRecGetLatest(char *id, CAMERA_RECORD *rlCamArray)
{
    CAMERA_LIST_HDR    *hdrPtr;
    CAMERA_RECORD      *camPtr;
    int                 i, count;

    if (DebugLevel >= DEBUG_INFO) {
        fprintf(DebugFP, "camRecGetLatest(\"%s\", 0x%lx[])\n",
                id, (long)rlCamArray);
    }

    // first record for any ids?
    if ( ! camLists) {
        return -1;       // no records of any kind
    }

    hdrPtr     = camListGetHdrById(id);

    if ( ! hdrPtr) {
        return -1;      // that id not found
    }

    zeroCamRecord(&rlCamArray[0]);
    zeroCamRecord(&rlCamArray[1]);

    count = 0;

    for (i = 0 ; i < NUM_OF_CAMERAS ; i++ ) {
        if ( ! hdrPtr->recs[i]) {          // but no camera records
            if (DebugLevel == DEBUG_DETAIL) {
                fprintf(DebugFP, "camRecGetLatest(): no \"%s\"camera records.\n",
                        i == CAMERA_LEFT_IDX ? "LEFT" : "RIGHT");
            }
        } else {
            rlCamArray[i] = *(hdrPtr->recs[i]);
            count++;
        }
    }

    return count;
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
camRecGetAvg(char *id, CAMERA_RECORD *rlCamArray)
{
    CAMERA_LIST_HDR    *hdrPtr;
    CAMERA_RECORD      *camPtr;
    int                 i, counts[NUM_OF_CAMERAS];

    if (DebugLevel >= DEBUG_INFO) {
        fprintf(DebugFP, "camRecGetAvg(\"%s\", 0x%lx[])\n",
                id, (long)rlCamArray);
    }

    // first record for any ids?
    if ( ! camLists) {
        return -1;       // no records of any kind
    }

    hdrPtr = camListGetHdrById(id);

    if ( ! hdrPtr) {
        return -1;      // that id not found
    }

    zeroCamRecord(&rlCamArray[0]);
    zeroCamRecord(&rlCamArray[1]);

    rlCamArray[CAMERA_LEFT_IDX].camera =  'L';
    rlCamArray[CAMERA_RIGHT_IDX].camera = 'R';

    counts[CAMERA_LEFT_IDX] = counts[CAMERA_RIGHT_IDX] = 0;

    for (i = 0 ; i < NUM_OF_CAMERAS ; i++ ) {
        if ( ! hdrPtr->recs[i]) {          // but no camera records
            if (DebugLevel == DEBUG_DETAIL) {
                fprintf(DebugFP, "camRecGetAvg(): no \"%s\"camera records.\n",
                        i == CAMERA_LEFT_IDX ? "LEFT" : "RIGHT");
            }
        } else {                // there are recors to walk each camera's list
            camPtr = hdrPtr->recs[i];

            while (camPtr) {    // walk the list for each camera
                rlCamArray[i].x += camPtr->x;
                rlCamArray[i].y += camPtr->y;
                rlCamArray[i].w += camPtr->w;
                rlCamArray[i].h += camPtr->h;

                counts[i]++;    // increment the counter for this camera
 
                camPtr = camPtr->next;
            }
        }
    }

    // been through the whole list so calculate the averages for each
    for (i = 0 ; i < NUM_OF_CAMERAS ; i++) {
        if (counts[i]) {        // only do the average if we found something
            rlCamArray[i].x  /= counts[i];;
            rlCamArray[i].y  /= counts[i];;
            rlCamArray[i].w  /= counts[i];;
            rlCamArray[i].h  /= counts[i];;
        }
    }

    if (counts[CAMERA_LEFT_IDX] && counts[CAMERA_RIGHT_IDX]) {
        return 2;
    }

    if (counts[CAMERA_LEFT_IDX] || counts[CAMERA_RIGHT_IDX]) {
        return 1;
    } else {
        return 0;
    }
}    






void
dumpLists()
{
    CAMERA_LIST_HDR *camListHdr = camLists;
    int             i;

    fprintf(DebugFP, "dumpLists():\nv----------------------v\n");

    if (! camLists ) {
        fprintf(DebugFP, "\tNO CAMERA RECORDS\n");
        fprintf(DebugFP, "^------------^\n");
        return;
    }

    while (camListHdr) {
        if (DebugLevel == DEBUG_DETAIL) {
            dumpCamListHdr(camListHdr);
        }

        for (i = 0 ; i < NUM_OF_CAMERAS ; i++) {

            CAMERA_RECORD  *camRecs = camListHdr->recs[i];

            while (camRecs) {
                if (DebugLevel == DEBUG_DETAIL) {
                    dumpCamRecord(camRecs);
                }

                camRecs = camRecs->next;
            }
        }
        camListHdr = camListHdr->next;
    }
    fprintf(DebugFP, "^----------------------^\n");
}


/// \brief dump an individual CAMERA_LIST_HDR object to the debug output
///
void
dumpCamListHdr(CAMERA_LIST_HDR *ptr)
{
    fprintf(DebugFP, "CAMERA_LIST_HDR @ (0x%lx):\n", (long)ptr);

    fprintf(DebugFP, "\tID:\t\"%s\"\n\tRecs:\t(0x%lx [LEFT]) (0x%lx [RIGHT])\n\tNext:\t(0x%lx)\n",
            ptr->id,
            (long)ptr->recs[CAMERA_LEFT_IDX], (long)ptr->recs[CAMERA_RIGHT_IDX],
            (long)ptr->next);
}



/// \brief dump an individual CAMERA_RECORD object to the debug output
///
void
dumpCamRecord(CAMERA_RECORD *ptr)
{
    fprintf(DebugFP, "CAMERA_RECORD @ (0x%lx):\n", (long)ptr);

    fprintf(DebugFP, "\tTime:\t%ld.%08ld\n\tCamera:\t%c\n\tx, y:\t%d, %d\n\tw, h:\t%d, %d\n\tNext:\t(0x%lx)\n",
            ptr->time.tv_sec, ptr->time.tv_usec, ptr->camera,
            ptr->x, ptr->y, ptr->w, ptr->h,
            (long)ptr->next);
}




/// \brief zero all fields in an individual CAMERA_RECORD object to the debug output
///
void
zeroCamRecord(CAMERA_RECORD *ptr)
{
    ptr->time.tv_sec = ptr->time.tv_usec = 0;
    ptr->camera = ' ';
    ptr->x = ptr->y = ptr->w = ptr->h = 0;
    ptr->next = (CAMERA_RECORD *)NULL;
}
