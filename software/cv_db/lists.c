//    cv_lists.c
//
//    manipulation functions for camera data lists

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "db/externs.h"
#include "db/lists.h"



#ifdef  DEBUG // show function and line in debug output --------------------------V

#define LOCK_SENSOR_LIST   \
                        if (DebugLevel == DEBUG_DETAIL) {\
                            fprintf(DebugFP, "Line %d in %s: getLock(0x%lx)\n", \
                                    __LINE__, __func__, (long)&camListLock); \
                        } getLock(&sensorListLock)

#define UNLOCK_SENSOR_LIST \
                        if (DebugLevel == DEBUG_DETAIL) {\
                            fprintf(DebugFP, "Line %d in %s: releaseLock(0x%lx)\n", \
                                    __LINE__, __func__, (long)&camListLock); \
                        } releaseLock(&sensorListLock)

#else   //  ! DEBUG ----------------------------------------------------------------

#define LOCK_SENSOR_LIST    getLock(&mainListLock)
#define UNLOCK_SENSOR_LIST  releaseLock(&mainListLock)

#endif  //  DEBUG -----------------------------------------------------------------^



static int              getLock(pthread_mutex_t *),
                        releaseLock(pthread_mutex_t *);

static pthread_mutex_t  mainListLock;



static SENSOR_LIST      *sensorListGetHead();

static SENSOR_LIST      *SensorLists = (SENSOR_LIST *)NULL;


static void             pruneByHdr(CAMERA_LIST_HDR *, int),
                        freeCamRecsFromEnd(CAMERA_RECORD *); // RECURSIVE!!




/// \brief  return the sensorList hdr
///
/// internal support routine
///
/// doesn't lock sensorList - assumes caller has locked it (if needed)
static SENSOR_LIST *
sensorListGetHead()
{
    return(SensorLists);
}


/// \brief  Get pointer to a SENSOR_LIST for a specific sensor type
///
/// type
///
/// doesn't lock the sensorlist - at worst it will miss the first entry
/// which could be added while we are traversing
///
/// no elements ever get removed from the camList headers
SENSOR_LIST *
sensorListGetPtrBySensor(SENSOR_TYPE sensor)
{
    SENSOR_LIST   *ptr;

    // any sensors at all?
    if ( ! (ptr = sensorListGetHdr())) {
        return NULL;
    }

    // look for the sensor type
    while (ptr) {
        if (ptr->type == sensor) {
            return ptr;
        }

        ptr = ptr->next;
    }

    return (SENSOR_LIST *)NULL;
}


/// \brief  Get pointer to a SENSOR_ID_LIST for a specific sensor
///         and id
///
/// type, id
///
/// doesn't lock the camlist - at worst it will miss the first entry
/// which could be added while we are traversing
///
/// no elements ever get removed from the camList headers
SENSOR_ID_LIST *
sensorIdListGetPtrBySensorId(SENSOR_TYPE sensor, char *id)
{
    SENSOR_LIST *ptr;

    if (ptr = sensorListGetPtrBySensor(sensor)) {
        return (SENSOR_ID_LIST *)NULL;
    }

    // look for the sensor id
    while (ptr) {
        if (strcmp(id, ptr->id) == 0) {
            return ptr;
        }

        ptr = ptr->next;
    }

    return (SENSOR_ID_LIST *)NULL;
}




/// \brief  Get pointer to a SENSOR_SUBID_LIST for a specific sensor,
///         id, and subid
///
/// type, id
///
/// doesn't lock the camlist - at worst it will miss the first entry
/// which could be added while we are traversing
///
/// no elements ever get removed from the camList headers
SENSOR_SUBID_LIST *
sensorIdListGetPtrBySensorIdSubid(SENSOR_TYPE sensor, char *id, char *subId)
{
    SENSOR_ID_LIST  *ptr;

    if (ptr = sensorIdListGetPtrBySensorId(sensor, id)) {
        return (SENSOR_SUBID_LIST *)NULL;
    }

    // look for the sensor subid
    while (ptr) {
        if (strcmp(subId, ptr->subId) == 0) {
            return ptr;
        }

        ptr = ptr->next;
    }

    return (SENSOR_ID_LIST *)NULL;
}

XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// \brief  Add a sensor record
///
/// type, id, subId, #, #, #, #
///
/// locks the camList
int
sensorRecAdd(SENSOR_TYPE sensor, char *id, char *subId, int x, int y, int w, int h)
{
    SENSOR_SUBID_LIST   *subIdPtr;
    SENSOR_ID_LIST      *idPtr;
    SENSOR_LIST         *sensorPtr;
    
    if (DebugLevel == DEBUG_INFO) {
        fprintf(DebugFP, "%s: %s(%d, \"%s\", \"%s\", %d, %d, %d, %d)\n",
                MyName, __func__, (int)sensor, id, subId, x, y, w, h);
    }

    LOCK_SENSOR_LIST;

    // can we just add a record - everything else in place?
 
CAN WE CHECK AND ADD IN REVERSE????

    START SEEING FROM THE LOWEST LEVEL IF SOMETHING EXISTS AND ADD TO WHAT WE DO FIND?

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
    hdrPtr = sensorListGetHdrById(id);

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

    UNLOCK_SENSOR_LIST;

    return 1;
}




/// \brief  remove any camera records that are beyond the ttl (in secs) for a specific object only
///
/// The list of camera records is ordered with the newest at the front
/// so once we find one that is past the TTL, they are all past the TTL from
/// that point onward
///
/// locks the camlist
void
camRecPruneById(char *id, int ttl)
{
    CAMERA_LIST_HDR *hdrPtr;

    if (ttl == 0) {
        return;         // no pruning if TTL is 0
    }

    LOCK_SENSOR_LIST;

    if ((hdrPtr = sensorListGetHdrById(id)) == 0) {
        UNLOCK_SENSOR_LIST;
        return;         // specified object not found
    }

    while (hdrPtr) {
        if (strcmp(id, hdrPtr->id) == 0) {  // found it
            pruneByHdr(hdrPtr, ttl);

            UNLOCK_SENSOR_LIST;
            return;
        }

        hdrPtr = hdrPtr->next;
    }

    UNLOCK_SENSOR_LIST;
}




/// \brief  remove any camera records that are beyond the ttl (in secs)
///
/// The list of camera records is ordered with the newest at the front
/// so once we find one that is past the TTL, they are all past the TTL from
/// that point onward
///
/// locaks the camList
void
camRecPruneAll(int ttl)
{
    CAMERA_LIST_HDR    *hdrPtr;

    if (ttl == 0) {
        return;         // no pruning if TTL is 0
    }
#ifdef  DEBUG
    // This locks the list so it must precede the LOCK_SENSOR_LIST below
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%d): prior to prune\n", __func__, ttl);
        dumpLists();
    }
#endif  // DEBUG

    LOCK_SENSOR_LIST;

    if ((hdrPtr = sensorListGetHdr()) == (CAMERA_LIST_HDR *)NULL) {
        UNLOCK_SENSOR_LIST;
        return;               // the whole list is empty
    }

    // walk the list of headers
    while (hdrPtr) {
        // check each object header and prune as needed
        pruneByHdr(hdrPtr, ttl);

        hdrPtr = hdrPtr->next;
    }

    UNLOCK_SENSOR_LIST;
}


/// \brief  prune both cameras for a given header record
///
/// internal list support routine
///
/// doesn't lock camList - assumes caller locked it
static void
pruneByHdr(CAMERA_LIST_HDR *hdrPtr, int ttl)
{
    CAMERA_RECORD   *camPtr, *camPtrPrev;;
    struct timeval  tvNow, tvDiff;
    int             i;

    gettimeofday(&tvNow, (struct timezone *)NULL);     // time now
#ifdef  DEBUG
    if (DebugLevel == DEBUG_DETAIL) {
#ifdef	__APPLE__
        fprintf(DebugFP, "%s(0x%lx, %d): time is now %ld.%d\n",
#else
        fprintf(DebugFP, "%s(0x%lx, %d): time is now %ld.%ld\n",
#endif
                __func__, (long)hdrPtr, ttl, tvNow.tv_sec, tvNow.tv_usec);
    }
#endif  // DEBUG

    // walk down the camera recs (if there are any)
    // need to go down the right and the left camera lists
    for (i = 0 ; i < NUM_OF_CAMERAS ; i++) {
            
        camPtr = camPtrPrev = hdrPtr->recs[i];

        while (camPtr) {

            timersub(&tvNow, &(camPtr->time), &tvDiff);

            if (tvDiff.tv_sec >= ttl) {     // too old and so are the rest

                if (camPtr == hdrPtr->recs[i]) {
 
                    // all of them starting from the newest are old so just
                    // unhook them all
                    hdrPtr->recs[i] = (CAMERA_RECORD *)NULL;

                } else {

                    // unhook from the previous record
                    camPtrPrev->next = (CAMERA_RECORD *)NULL;
                }

                // now do a little recursion to free all of them
                // from the end backwards

                freeCamRecsFromEnd(camPtr);
            }

            camPtrPrev = camPtr;
            camPtr = camPtr->next;
        }

        if ((camPtr = hdrPtr->recs[i])) {
            // they were all aged out, right from the first one so the list
            // shoud now be completely empty - clear the point in the header
            hdrPtr->recs[i] = (CAMERA_RECORD *)NULL;
        }
    }
}



/// \brief frees all camera records starting at some location in the list
///
/// records are freed from the end of the list popping back towards the first
/// on to be purged
///
/// internal list support routine
///
/// doesn't lock camList - assumes caller did
static void
freeCamRecsFromEnd(CAMERA_RECORD *ptr)
{

#ifdef  DEBUG
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx) entered\n", __func__, (long)ptr);
    }
#endif  // DEBUG

    if (ptr->next) {    // keep going
        freeCamRecsFromEnd(ptr->next);  // RECURSIVE!!!
    }

    cvFree(ptr);    // free ME!
}



/// \brief  removes all camera recors for a specific object id
///
/// locks camList
int
camRecDeleteById(char *id)
{

    LOCK_SENSOR_LIST;

    UNLOCK_SENSOR_LIST;

    return 0;
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
///
/// locks camList
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

    LOCK_SENSOR_LIST;

    hdrPtr     = sensorListGetHdrById(id);

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

    UNLOCK_SENSOR_LIST;
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
///
/// locks camlist
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

    LOCK_SENSOR_LIST;

    hdrPtr = sensorListGetHdrById(id);

    if ( ! hdrPtr) {
        UNLOCK_SENSOR_LIST;
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
        UNLOCK_SENSOR_LIST;
        return 2;
    }

    if (counts[CAMERA_LEFT_IDX] || counts[CAMERA_RIGHT_IDX]) {
        UNLOCK_SENSOR_LIST;
        return 1;
    } else {
        UNLOCK_SENSOR_LIST;
        return 0;
    }
}    


static void
dumpSensorList(SENSOR_LIST *ptr)
{
    fprintf(DebugFP, "SENSOR_LIST(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\tType:\t\"%c\"\n", ptr->type);
    fprintf(DebugFP, "\tSensors:\t0x%lx\n", (long)ptr->sensors);
    fprintf(DebugFP, "\tNext:\t0x%lx\n\n", (long)ptr->next);
}


static void
dumpSensorIdList(SENSOR_ID_LIST *ptr)
{
    fprintf(DebugFP, "SENSOR_ID_LIST(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\tID:\t\"%s\"\n", ptr->id);
    fprintf(DebugFP, "\tSubIDs:\t0x%lx\n", (long)ptr->subIds);
    fprintf(DebugFP, "\tNext:\t0x%lx\n\n", (long)ptr->next);
}


static void
dumpSensorSubIdList(SENSOR_SUBID_LIST *ptr)
{
    fprintf(DebugFP, "SENSOR_SUBID_LIST(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\tSubID:\t\"%s\"\n", ptr->subId);
    fprintf(DebugFP, "\tData:\t0x%lx\n", (long)ptr->data);
    fprintf(DebugFP, "\tNext:\t0x%lx\n\n", (long)ptr->next);
}


static void
dumpSensorRecord(SENSOR_RECORD *ptr)
{
    fprintf(DebugFP, "SENSOR_RECORD(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\ttime:\t%ld.%08ld\n\n", ptr->time.tv_sec, ptr->time.tv_usec);
    fprintf(DebugFP, "\tType:\t%d ", ptr->type);

    switch (ptr->type) {

        case SENSOR_CAMERA:
           fprintf(DebugFP, "(Camera)\n");
           fprintf(DebugFP, "\t\tX:\t%d\n", ptr->sensorData.camera.x);
           fprintf(DebugFP, "\t\tY:\t%d\n", ptr->sensorData.camera.x);
           fprintf(DebugFP, "\t\tW:\t%d\n", ptr->sensorData.camera.w);
           fprintf(DebugFP, "\t\tH:\t%d\n", ptr->sensorData.camera.h);
           break;

        case SENSOR_RANGE:
           fprintf(DebugFP, "(Range)\n");
           fprintf(DebugFP, "\t\tRange:\t%d\n", ptr->sensorData.range.range);
           break;

        case SENSOR_ACCELL:
           fprintf(DebugFP, "(Acceleration)\n");
           fprintf(DebugFP, "\t\tX:\t%d\n", ptr->sensorData.accell.x);
           fprintf(DebugFP, "\t\tY:\t%d\n", ptr->sensorData.accell.y);
           fprintf(DebugFP, "\t\tZ:\t%d\n", ptr->sensorData.accell.z);
           break;

        case SENSOR_ROLL:
           fprintf(DebugFP, "(Roll)\n");
           fprintf(DebugFP, "\t\tX:\t%d\n", ptr->sensorData.roll.x);
           fprintf(DebugFP, "\t\tY:\t%d\n", ptr->sensorData.roll.y);
           fprintf(DebugFP, "\t\tZ:\t%d\n", ptr->sensorData.roll.z);
           break;

        case SENSOR_MAGNETIC:
           fprintf(DebugFP, "(Magnetic)\n");
           fprintf(DebugFP, "\t\tX:\t%d\n", ptr->sensorData.magnetic.x);
           fprintf(DebugFP, "\t\tY:\t%d\n", ptr->sensorData.magnetic.y);
           fprintf(DebugFP, "\t\tZ:\t%d\n", ptr->sensorData.magnetic.z);
           break;
    }

    fprintf(DebugFP, "\tNext:\t0x%lx\n\n", (long)ptr->next);
}


void
walkSensorRecords(SENSOR_RECORD *ptr)
{
    while (ptr) {
        dumpSensorRecord(ptr);
        
        ptr = ptr->next;
    }
}



void
walkSensorSubIdList(SENSOR_SUBID_LIST *ptr)
{
    while (ptr) {
        dumpSensorSubIdList(ptr);
        
        walkSensorRecords(ptr->data);

        ptr = ptr->next;
    }
}



void
walkSensorIdList(SENSOR_ID_LIST *ptr)
{
    while (ptr) {
        dumpSensorIdList(ptr);
        
        walkSensorSubIdList(ptr->subIds);

        ptr = ptr->next;
    }
}


void
walkSensorList(SENSOR_LIST *ptr)
{
    while (ptr) {
        dumpSensorList(ptr);
        
        walkSensorIdList(ptr->sensors);

        ptr = ptr->next;
    }
}


void
dumpLists()
{
    SENSOR_LIST         *sensorListPtr;

    fprintf(DebugFP, "dumpLists():\nVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV\n");

    LOCK_SENSOR_LIST;

    if ( ! (sensorListPtr = sensorListGetHead())) {
        fprintf(DebugFP, "\nNO RECORDS\n\n");
        return;
    } else {
        walkSensorList(sensorListPtr);
    }

    UNLOCK_SENSOR_LIST;

    fprintf(DebugFP, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}



/// \brief zero all fields in an individual SENSOR_LIST record
///
/// general utility function - useful for testing as well
void
zeroSensorListRecord(SENSOR_LIST *ptr)
{
    ptr->type    = 0;
    ptr->sensors = NULL;
    ptr->next     = NULL;
}



/// \brief zero all fields in an individual SENSOR_ID_LIST record
///
/// general utility function - useful for testing as well
void
zeroSensorIdListRecord(SENSOR_ID_LIST *ptr)
{
    ptr->id     = NULL;
    ptr->subIds = NULL;
    ptr->next    = NULL;
}



/// \brief zero all fields in an individual SENSOR_SUBID_LIST record
///
/// general utility function - useful for testing as well
void
zeroSensorSubIdListRecord(SENSOR_SUBID_LIST *ptr)
{
    ptr->subId = NULL;
    ptr->data  = NULL;
    ptr->next   = NULL;
}



/// \brief zero all fields in an individual SENSOR_RECORD record
///
/// does not zero any of the sensor-specific fields
///
/// general utility function - useful for testing as well
void
zeroSensorRecord(SENSOR_RECORD *ptr)
{
    ptr->time.tv_sec  = 0;
    ptr->time.tv_usec = 0;
    ptr->next          = NULL;
}



/// alloc and zero a SENSOR_LIST record
///
/// if a memory allocation error happens, program will exit
SENSOR_LIST *
allocSensorListRecord()
{
    SENSOR_LIST *ptr;

    ptr = cvAlloc(sizeof(SENSOR_LIST));

    zeroSensorListRecord(ptr);

    return ptr;
}



/// alloc and zero a SENSOR_ID_LIST record
///
/// we do set the id since we have to allocate memory for it
///
/// if a memory allocation error happens, program will exit
SENSOR_ID_LIST *
allocSensorIdListRecord(char *id)
{
    SENSOR_ID_LIST *ptr;

    ptr = cvAlloc(sizeof(SENSOR_ID_LIST));

    zeroSensorIdListRecord(ptr);

    ptr->id = cvAlloc(strlen(id) + 1);
    strcpy(ptr->id, id);

    return ptr;
}



/// alloc and zero a SENSOR_SUBID_LIST record
///
/// we do set the subId since we have to allocate memory for it
///
/// if a memory allocation error happens, program will exit
SENSOR_SUBID_LIST *
allocSensorSubIdListRecord(char *subId)
{
    SENSOR_SUBID_LIST *ptr;

    ptr = cvAlloc(sizeof(SENSOR_SUBID_LIST));

    zeroSensorSubIdListRecord(ptr);

    ptr->subId = cvAlloc(strlen(subId) + 1);
    strcpy(ptr->subId, subId);

    return ptr;
}



/// alloc and zero a SENSOR_ID_LIST record
///
/// if a memory allocation error happens, program will exit
SENSOR_RECORD *
allocSensorRecord()
{
    SENSOR_RECORD *ptr;

    ptr = cvAlloc(sizeof(SENSOR_RECORD));

    zeroSensorRecord(ptr);

    return ptr;
}


NEED FREE ROUTINES!!!!!

INCLUDING STUFF ALLOCATED FOR RECORDS (LIKE IDS, ETC).


/// \brief initialize the mutex for the coordinated sensor list access
//
void
initMutexes()
{
    int retVal;

    retVal = pthread_mutex_init(&mainListLock, (pthread_mutexattr_t *) NULL);
}





/// \brief try to get the specified lock
///
/// try to get the lock
/// if unsuccesful, try configured nbumber of times with configured
/// msec sleep between locks
///
/// returns 0 if successful - non-zero if unsuccessful
///
static int
getLock(pthread_mutex_t *lock)
{
    int i, retVal;

    for (i = 0 ; i < LOCK_MAX_ATTEMPTS ; i++) {
        if ((retVal = pthread_mutex_lock(lock)) == 0) {    // locked
#ifdef  DEBUG
            if (DebugLevel == DEBUG_DETAIL) {
                fprintf(DebugFP, "%s(0x%lx): took %d attempts to get lock\n",
                        __func__, (long)lock, i);
            }
#endif  // DEBUG
            return 0;
        }
#ifdef  DEBUG	// BE CAREFUL!  DEBUG adds 'else' to 'if' above!!
          else {

              char    *errorString;

              errorString = strerror(errno);

              if (DebugLevel == DEBUG_DETAIL) {
                  fprintf(DebugFP, "%s(0x%lx): pthread_mutex_lock() returned %d (%s)\n", __func__,
                         (long)lock, retVal, (retVal != 0) ? errorString : "Success");
              }
        }
#endif  // DEBUG

        
        usleep ((useconds_t) LOCK_USLEEP_TIME);
    }

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx): warning: could not get lock after %d attempts\n",
                MyName, (long)lock, i);
    }

    return 1;                                   // couldn't get lock
}




/// \brief try release the specified lock
///
/// returns 0 if successful - non-zero if unsuccessful
///
static int
releaseLock(pthread_mutex_t *lock)
{
	int     retVal;

    retVal =  pthread_mutex_unlock(lock);

#ifdef  DEBUG
    errorString = strerror(errno);

    if (DebugLevel == DEBUG_DETAIL && retVal) {
        fprintf(DebugFP, "%s(0x%lx): pthread_mutex_unlock() returned %d (%s)\n", __func__,
               (long)lock, retVal, (retVal != 0) ? errorString : "Success");
    }
#endif  // DEBUG

    return retVal;
}
