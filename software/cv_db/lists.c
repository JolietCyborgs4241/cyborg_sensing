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
#include <assert.h>

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



static pthread_mutex_t  mainListLock;

static int              getLock(pthread_mutex_t *),
                        releaseLock(pthread_mutex_t *);


static SENSOR_LIST          *allocSensorListRecord(), *sensorListGetHead();
static SENSOR_ID_LIST       *allocSensorIdRecord();
static SENSOR_SUBID_LIST    *allocSensorSubIdRecord();
static SENSOR_RECORD        *allocSensorRecord();

static SENSOR_LIST          *SensorLists = (SENSOR_LIST *)NULL;


static void                 freeSensorRecsFromEnd(SENSOR_RECORD *); // RECURSIVE!!




/// \brief  initialize the database
///
/// one record for each sensor type
void
initDb()
{
    SENSOR_TYPE sensors[] = { SENSOR_CAMERA,
                              SENSOR_RANGE,
                              SENSOR_ACCELL,
                              SENSOR_ROLL,
                              SENSOR_MAGNETIC,
                              0 };
    SENSOR_TYPE *sensorPtr = sensors;
    SENSOR_LIST *ptr, *prevPtr, *firstPtr;
    int         firstOne = 1;

    prevPtr = (SENSOR_LIST *)NULL;

    while (*sensorPtr) {
        ptr = allocSensorListRecord();

        if (firstOne) {
            firstPtr = ptr;
            firstOne = 0;
        }

        ptr->type = *sensorPtr++;

        if (prevPtr) {
            prevPtr->next = ptr;
        }

        prevPtr = ptr;
    }

    SensorLists = firstPtr;

    dumpLists();
}

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
    if ( ! (ptr = sensorListGetHead())) {
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
    SENSOR_LIST     *ptr;
    SENSOR_ID_LIST  *idPtr;

    if (ptr = sensorListGetPtrBySensor(sensor)) {
        return (SENSOR_ID_LIST *)NULL;
    }

    idPtr = ptr->sensors;

    // look for the sensor id
    while (idPtr) {
        if (strcmp(id, idPtr->id) == 0) {
            return idPtr;
        }

        idPtr = idPtr->next;
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
    SENSOR_ID_LIST      *ptr;
    SENSOR_SUBID_LIST   *subIdPtr;

    if (ptr = sensorIdListGetPtrBySensorId(sensor, id)) {
        return (SENSOR_SUBID_LIST *)NULL;
    }

    subIdPtr = ptr->subIds;

    // look for the sensor subid
    while (subIdPtr) {
        if (strcmp(subId, subIdPtr->subId) == 0) {
            return subIdPtr;
        }

        subIdPtr = subIdPtr->next;
    }

    return (SENSOR_SUBID_LIST *)NULL;
}



/// \brief  Add a sensor record
///
/// type, id, subId, #, #, #, #
///
/// locks the camList
int
sensorRecAdd(SENSOR_TYPE sensor, char *id, char *subId, int x, int y, int w, int h)
{
#ifdef NEVER
    SENSOR_LIST         *listPtr;
    SENSOR_SUBID_LIST   *subIdPtr;
    SENSOR_ID_LIST      *idPtr;
    SENSOR_LIST         *sensorPtr;
    
    if (DebugLevel == DEBUG_INFO) {
        fprintf(DebugFP, "%s: %s(%d, \"%s\", \"%s\", %d, %d, %d, %d)\n",
                MyName, __func__, (int)sensor, id, subId, x, y, w, h);
    }

    LOCK_SENSOR_LIST;

    listPtr = sensorListGetHead();

    while (listPtr) {

    }

    if ( !
#endif
}


void
sensorRecPruneAll(int ttl)
{
#warning    sensorRecPruneAll implementation
}


/// \brief frees all sensor records starting at some location in the list
///
/// records are freed from the end of the list popping back towards the first
/// on to be purged
///
/// internal list support routine
///
/// doesn't lock list - assumes caller did
static void
freeSensorRecsFromEnd(SENSOR_RECORD *ptr)
{

#ifdef  DEBUG
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx) entered\n", __func__, (long)ptr);
    }
#endif  // DEBUG

    if (ptr->next) {    // keep going
        freeSensorRecsFromEnd(ptr->next);  // RECURSIVE!!!
    }

    cvFree(ptr);    // free ME!
}



// *****************************************************************
//
// dump routines
//
// *****************************************************************

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



// *****************************************************************
//
// walk routines
//
// *****************************************************************


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



// *****************************************************************
//
// zero routines
//
// *****************************************************************


/// \brief zero all fields in an individual SENSOR_LIST record
///
/// general utility function - useful for testing as well
static void
zeroSensorListRecord(SENSOR_LIST *ptr)
{
    ptr->type    = 0;
    ptr->sensors = NULL;
    ptr->next     = NULL;
}



/// \brief zero all fields in an individual SENSOR_ID_LIST record
///
/// general utility function - useful for testing as well
static void
zeroSensorIdListRecord(SENSOR_ID_LIST *ptr)
{
    ptr->id     = NULL;
    ptr->subIds = NULL;
    ptr->next    = NULL;
}



/// \brief zero all fields in an individual SENSOR_SUBID_LIST record
///
/// general utility function - useful for testing as well
static void
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
static void
zeroSensorRecord(SENSOR_RECORD *ptr)
{
    ptr->time.tv_sec  = 0;
    ptr->time.tv_usec = 0;
    ptr->next          = NULL;
}



// *****************************************************************
//
// alloc routines
//
// *****************************************************************


/// alloc and zero a SENSOR_LIST record
///
/// if a memory allocation error happens, program will exit
static SENSOR_LIST *
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
static SENSOR_ID_LIST *
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
static SENSOR_SUBID_LIST *
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
static SENSOR_RECORD *
allocSensorRecord()
{
    SENSOR_RECORD *ptr;

    ptr = cvAlloc(sizeof(SENSOR_RECORD));

    zeroSensorRecord(ptr);

    return ptr;
}



// *****************************************************************
//
// free routines
//
// *****************************************************************

//  ***************************************************************************
//  No SENSOR_LIST free routine - we don't free them so they are around for the
//  life of the database
//  ***************************************************************************

/// free SENSOR_ID_LIST record
///
/// free id as well
void
freeSensorIdListRecord(SENSOR_ID_LIST *ptr)
{
    assert( ! ptr);
    assert( ! ptr->id);

    cvFree(ptr->id);
    cvFree(ptr);
}



/// free SENSOR_SUBID_LIST record
///
/// frees subId string as well
void
freeSensorSubIdListRecord(SENSOR_SUBID_LIST *ptr)
{
    assert (! ptr);
    assert( ! ptr->subId);

    cvFree(ptr->subId);
    cvFree(ptr);
}



/// free a SENSOR_RECORD record
///
void
FreeSensorRecord(SENSOR_RECORD *ptr)
{
    assert( ! ptr);

    cvFree(ptr);
}




// *****************************************************************
//
// lock management routines
//
// *****************************************************************


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



/// \brief release the specified lock
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
