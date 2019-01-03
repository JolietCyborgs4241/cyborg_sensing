//    cv_lists.c
//
//    manipulation functions for sensor data lists

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/time.h>
#include <assert.h>
#include <signal.h>

#include "cv.h"
#include "cv_net.h"
#include "sensors.h"
#include "db/externs.h"
#include "db/lists.h"
#include "db/query.h"



#ifdef  LOCK_DEBUG // show function and line in debug output --------------------------V

#define LOCK_SENSOR_LIST   \
        fprintf(DebugFP, "Line %d in %s: getLock(0x%lx)\n", \
                __LINE__, __func__, (long)&mainListLock); \
        getLock(&mainListLock)

#define UNLOCK_SENSOR_LIST \
        fprintf(DebugFP, "Line %d in %s: releaseLock(0x%lx)\n", \
                __LINE__, __func__, (long)&mainListLock); \
        releaseLock(&mainListLock)

#else   //  ! LOCK_DEBUG ----------------------------------------------------------------

#define LOCK_SENSOR_LIST    getLock(&mainListLock)
#define UNLOCK_SENSOR_LIST  releaseLock(&mainListLock)

#endif  //  LOCK_DEBUG -----------------------------------------------------------------^



static pthread_mutex_t      mainListLock = PTHREAD_MUTEX_INITIALIZER;

static void                 getLock(pthread_mutex_t *),
                            releaseLock(pthread_mutex_t *);


static SENSOR_LIST          *allocSensorListRecord(), *sensorListGetHead();
static SENSOR_ID_LIST       *allocSensorIdListRecord(char *);
static SENSOR_SUBID_LIST    *allocSensorSubIdListRecord(char *);
static SENSOR_RECORD        *allocSensorRecord();


static SENSOR_LIST          *SensorLists = (SENSOR_LIST *)NULL;


static void                 freeSensorRecsFromEnd(SENSOR_RECORD *); // RECURSIVE!!




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



// *****************************************************************
//
// Initilize DB - create one top level record per sensor
//
// *****************************************************************


// dumpList on signal USR1
static void
dumpOnUsr1Signal(int i, siginfo_t *info, void *ptr)
{
    dumpListStats();    // debug level doesn't matter - it's on demand
}


// dumpList on signal USR2
static void
dumpOnUsr2Signal(int i, siginfo_t *info, void *ptr)
{
    dumpLists();        // debug level doesn't matter - it's on demand
}


/// \brief  initialize the database
///
/// one record for each sensor type at the top level
void
initDb()
{
    SENSOR_TYPE      sensors[] = { SENSOR_CAMERA,
                                   SENSOR_RANGE,
                                   SENSOR_ACCELL,
                                   SENSOR_ROLL,
                                   SENSOR_MAGNETIC,
                                   SENSOR_OBSTACLE,
                                   0 };
    SENSOR_TYPE      *sensorPtr = sensors;
    SENSOR_LIST      *ptr, *prevPtr, *firstPtr;
    struct sigaction sigHandling;
    int              firstOne = 1;




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

#ifdef NEVER
    sigHandling.sa_sigaction = &dumpOnUsr1Signal;
    sigaction(SIGUSR1, &sigHandling, NULL);

    sigHandling.sa_sigaction = &dumpOnUsr2Signal;
    sigaction(SIGUSR2, &sigHandling, NULL);
#endif // NEVER
}


/// \brief  Get pointer to a SENSOR_LIST for a specific sensor type
///
/// type
///
/// doesn't lock the sensorlist - at worst it will miss the first entry
/// which could be added while we are traversing
///
/// no elements ever get removed from the SENSOR_LIST headers
SENSOR_LIST *
sensorGetListBySensor(SENSOR_TYPE sensor)
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
/// doesn't lock the sensor list - at worst it will miss the first entry
/// which could be added while we are traversing
///
/// no elements ever get removed from the SENSOR_ID_LIST headers
SENSOR_ID_LIST *
sensorGetIdListById(SENSOR_TYPE sensor, char *id)
{
    SENSOR_LIST     *ptr;
    SENSOR_ID_LIST  *idPtr;

    if ((ptr = sensorGetListBySensor(sensor))) {
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
/// doesn't lock the sensor list
SENSOR_SUBID_LIST *
sensorSubIdListBySubid(SENSOR_TYPE sensor, char *id, char *subId)
{
    SENSOR_ID_LIST      *ptr;
    SENSOR_SUBID_LIST   *subIdPtr;

    if ((ptr = sensorGetIdListById(sensor, id))) {
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




// *****************************************************************
//
// add routines
//
// *****************************************************************

/// \brief  Alloc, fill, and add a sensor record to a subId record
static void
newSensorRecord(SENSOR_SUBID_LIST *subPtr, SENSOR_TYPE type,
                int i1, int i2, int i3, int i4)
{
    SENSOR_RECORD   *sensorPtr;

    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s((0x%lx), \'%c\', %d, %d, %d, %d)\n",
                __func__, (long)subPtr, type, i1, i2, i3, i4);
    }

    sensorPtr = allocSensorRecord();

    sensorPtr->type = type;             // save sensor type again
    sensorPtr->next = subPtr->data;     // hook to the front
    subPtr->data    = sensorPtr;        // update subId record
    gettimeofday(&(sensorPtr->time), NULL);

    switch (type) {

    case SENSOR_CAMERA:
        sensorPtr->rawData[CAMERA_X] = i1;
        sensorPtr->rawData[CAMERA_Y] = i2;
        sensorPtr->rawData[CAMERA_W] = i3;
        sensorPtr->rawData[CAMERA_H] = i4;
        break;

    case SENSOR_RANGE:
        sensorPtr->rawData[RANGE_X] = i1;
        break;

    case SENSOR_ACCELL:
        sensorPtr->rawData[ACCELL_X] = i1;
        sensorPtr->rawData[ACCELL_Y] = i2;
        sensorPtr->rawData[ACCELL_Z] = i3;
        break;

    case SENSOR_ROLL:
        sensorPtr->rawData[ROLL_X] = i1;
        sensorPtr->rawData[ROLL_Y] = i2;
        sensorPtr->rawData[ROLL_Z] = i3;
        break;

    case SENSOR_MAGNETIC:
        sensorPtr->rawData[MAGNETIC_X] = i1;
        sensorPtr->rawData[MAGNETIC_Y] = i2;
        sensorPtr->rawData[MAGNETIC_Z] = i3;
        break;
#warning add filtering and gain calculations to newSensorRecord
    }
}


/// \brief  Alloc, fill, and add a subid record to a id record
/// adding the sensor record as well
static void
newSensorSubId(SENSOR_ID_LIST *idPtr, SENSOR_TYPE type, char *subId,
               int i1, int i2, int i3, int i4)
{
    SENSOR_SUBID_LIST   *subIdPtr;

    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s((0x%lx), \'%c\', \"%s\", %d, %d, %d, %d\n",
		__func__, (long)idPtr, type, subId, i1, i2, i3, i4);
    }

    subIdPtr = allocSensorSubIdListRecord(subId);

    subIdPtr->next = idPtr->subIds;   // hook to the front
    idPtr->subIds  = subIdPtr;        // update id record

    newSensorRecord(subIdPtr, type, i1, i2, i3, i4);    // add the new sensor record
}



/// \brief  Alloc, fill, and add a id record to a top level sensor record
/// adding the id, subId, and sensor record as well
static void
newSensorId(SENSOR_LIST *listPtr, char *id, char *subId, SENSOR_TYPE type,
                int i1, int i2, int i3, int i4)
{
    SENSOR_ID_LIST   *idPtr;

    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s((0x%lx), \"%s\", \"%s\", \'%c\', %d, %d, %d, %d\n",
		__func__, (long)listPtr, id, subId, type, i1, i2, i3, i4);
    }

    idPtr = allocSensorIdListRecord(id);

    idPtr->next       = listPtr->sensors;   // hook to the front
    listPtr->sensors  = idPtr;              // update id record

    newSensorSubId(idPtr, type, subId, i1, i2, i3, i4);    // add the new subId record
}



/// \brief  Add a sensor record
///
/// type, id, subId, #, #, #, #
///
/// locks the sensor list
void
sensorRecAdd(SENSOR_TYPE sensor, char *id, char *subId, int i1, int i2, int i3, int i4)
{
    SENSOR_LIST         *listPtr;
    SENSOR_SUBID_LIST   *subIdPtr;
    SENSOR_ID_LIST      *idPtr;
    SENSOR_LIST         *sensorPtr;
    int                 subIdLen;
    
    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s(\'%c\', \"%s\", \"%s\", %d, %d, %d, %d)\n",
                __func__, (int)sensor, id, subId, i1, i2, i3, i4);
    }

    LOCK_SENSOR_LIST;

    if (DebugLevel == DEBUG_SUPER) {
        dumpLists();
    }

    // we'll go as far down the list as we can until we don't find something
    // and then add it

    listPtr = sensorListGetHead();

    while (listPtr) {
        if (listPtr->type == sensor) {  // found correct sensor
            idPtr = listPtr->sensors;   // down to the id level

            while (idPtr) {
                if (strcmp(idPtr->id, id) == 0) {   // found correct ID
                    subIdPtr = idPtr->subIds;       // down to the subId level

                    while (subIdPtr) {
                        subIdLen = strlen(subId);   // could be an empty string
                        if ((subIdLen == 0 && strlen(subIdPtr->subId) == 0) ||
                            (strcmp(subId, subIdPtr->subId) == 0)) {    // found subId

				            // looks like all we need to do is add the sensor data record
				            newSensorRecord(subIdPtr, sensor, i1, i2, i3, i4);

                            goto sensorAdded;
                        }
                        subIdPtr = subIdPtr->next;
                     }

                     // looks like we need to add the subId record as well
                     newSensorSubId(idPtr, sensor, subId, i1, i2, i3, i4);

                     goto sensorAdded;
                }

                idPtr = idPtr->next;
            }

            // looks like we need to add the id record as well
            newSensorId(listPtr, id, subId, sensor, i1, i2, i3, i4);

            goto sensorAdded;
        }

        listPtr = listPtr->next;
    }

    UNLOCK_SENSOR_LIST;

    // there should never be a case where we have a sensor type that's not already listed
    fprintf(DebugFP, "%s: warning: %s(%c, \"%s\", \"%s\", %d, %d, %d, %d\n",
            MyName, __func__, sensor, id, subId, i1, i2, i3, i4);
    return;

sensorAdded:

    UNLOCK_SENSOR_LIST;

    return;
}


void
sensorRecPruneAll()
{
    
    TTLS                *ttlPtr = SensorTtls;
    struct timeval      now, timeDiff;
    SENSOR_LIST         *sensorListPtr;
    SENSOR_ID_LIST      *sensorIdListPtr;
    SENSOR_SUBID_LIST   *sensorSubIdListPtr;
    SENSOR_RECORD       *sensorPtr, *prevSensorPtr;

    gettimeofday(&now, NULL);

    if (DebugLevel >= DEBUG_DETAIL) {
#ifdef  __APPLE__
        fprintf(DebugFP, "%s() enter @ %ld.%06d\n", __func__, now.tv_sec, now.tv_usec);
#else   // ! __APPLE__
        fprintf(DebugFP, "%s() enter @ %ld.%06ld\n", __func__, now.tv_sec, now.tv_usec);
#endif  // __APPLE__
    }

    LOCK_SENSOR_LIST;

    if (DebugLevel >= DEBUG_SUPER) {
        dumpLists();
    }

    while (ttlPtr->sensor) {   // go through sensor types in the TTL list

        if (DebugLevel >= DEBUG_SUPER) {
            fprintf(DebugFP, "%s(): pruning sensor \'%c\'\n", __func__, ttlPtr->sensor);
        }

        sensorListPtr = sensorGetListBySensor(ttlPtr->sensor);

        sensorIdListPtr = sensorListPtr->sensors;

        while (sensorIdListPtr) {           // walk the ids
            if (DebugLevel >= DEBUG_SUPER) {
                fprintf(DebugFP, "%s(): pruning SENSOR_ID @ (0x%lx)n",
                        __func__, (long)sensorIdListPtr);
            }

            sensorSubIdListPtr = sensorIdListPtr->subIds;

            while (sensorSubIdListPtr) {    // walk the subIds
 
                if (DebugLevel >= DEBUG_SUPER) {
                    fprintf(DebugFP, "%s(): pruning SENSOR_SUB_ID @ (0x%lx)n",
                            __func__, (long)sensorSubIdListPtr);
                }

                sensorPtr     = sensorSubIdListPtr->data;
                prevSensorPtr = sensorPtr;

                while (sensorPtr) {    // walk the sensor data records

                    if (DebugLevel >= DEBUG_SUPER) {
                        fprintf(DebugFP, "%s(): pruning SENSOR @ (0x%lx)n",
                                __func__, (long)sensorSubIdListPtr);
                    }

                    timersub(&now, &(sensorPtr->time), &timeDiff);

                    if (timeDiff.tv_sec > ttlPtr->ttlSecs ||
                        (timeDiff.tv_sec == ttlPtr->ttlSecs && timeDiff.tv_usec > ttlPtr->ttlUsecs)) {
                        // start pruning from here
                        if (prevSensorPtr == sensorPtr) {   // prune them all
                            sensorSubIdListPtr->data = NULL;
                        } else {
                            prevSensorPtr->next = NULL;
                       }

                        freeSensorRecsFromEnd(sensorPtr);

                        sensorPtr = NULL;

                    } else {

                        prevSensorPtr = sensorPtr;
                        sensorPtr     = sensorPtr->next;
                    }
                }

                sensorSubIdListPtr = sensorSubIdListPtr->next;
            }

            sensorIdListPtr = sensorIdListPtr->next;
        }

        ttlPtr++;       // next sensor id on the TTL list
    }

    UNLOCK_SENSOR_LIST;
}

// *****************************************************************
//
// query routines
//
// *****************************************************************

//  query return data placeholders

static int  RetCount;
static int  *RetValPtrs[MAX_QUERY_RET_VALS][MAX_SENSOR_VALUES];
static char *RetIdPtrs[MAX_QUERY_RET_VALS];
static char *RetSubIdPtrs[MAX_QUERY_RET_VALS];
static int  RetAvgSums[MAX_QUERY_RET_VALS][MAX_SENSOR_VALUES];
static int  RetAvgCounts[MAX_QUERY_RET_VALS];


/// \brief  process a query and return the results
///
/// use sensor type, id, subid to find sensor records
///
/// returns a record count as part of the response with the record contents
///
/// even is what seems like a singular request (Latest or Earliest) could
/// return multiple records if id or subId wildcards are specified
void
processQuery(char *tag, char query, char sensor, char *id, char *subId,
             char *retBuff, int retBuffSize)
{
    int i, ii;
    SENSOR_LIST         *sensorListPtr;
    SENSOR_ID_LIST      *sensorIdListPtr;
    SENSOR_SUBID_LIST   *sensorSubIdListPtr;
    SENSOR_RECORD       *sensorPtr, *prevSensorPtr;

    RetCount = 0;

    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s(\"%s\", \'%c\', \'%c\', \"%s\", \"%s\", 0x%lx, %d)\n",
                __func__, tag, query, sensor, id, subId, (long)retBuff, retBuffSize);
    }

    LOCK_SENSOR_LIST;

    // find that sensor
    sensorListPtr = sensorGetListBySensor(sensor);

    if ( ! sensorListPtr) {     // sensor not found
#ifdef NEVER
        if (DebugLevel >= DEBUG_INFO) {
            fprintf(DebugFP, "%s(): sensor \'%c\' not found!\n",
                    __func__, sensor);
        }
#endif

        sprintf(retBuff, "%s %c %c 0\n", tag, query, sensor);

        UNLOCK_SENSOR_LIST;

        return;
    }

#ifdef NEVER
    if (DebugLevel >= DEBUG_INFO) {
        fprintf(DebugFP, "%s(): sensor \'%c\' found - searching for id \"%s\"\n",
                __func__, sensor, id);
    }
#endif

    // find that id
    sensorIdListPtr = sensorListPtr->sensors;

    while (sensorIdListPtr) {
        if (*id == '*' || strcmp(id, sensorIdListPtr->id) == 0) {
#ifdef NEVER
            if (DebugLevel >= DEBUG_DETAIL) {
                fprintf(DebugFP, "%s(): sensor \'%c\' id \"%s\" found - searching for subId \"%s\"\n",
                        __func__, sensor, sensorIdListPtr->id, subId);
            }
#endif

            // find that subid
            sensorSubIdListPtr = sensorIdListPtr->subIds;

            while (sensorSubIdListPtr) {
                if (*subId == '*' || strcmp(subId, sensorSubIdListPtr->subId) == 0) {
#ifdef NEVER
                    if (DebugLevel >= DEBUG_DETAIL) {
                        fprintf(DebugFP,
                                "%s(): sensor \'%c\' id \"%s\" subId \"%s\" found!\n",
                                __func__, sensor, sensorIdListPtr->id,
                                sensorSubIdListPtr->subId);
                    }
#endif

                    if ( ! (sensorPtr = sensorSubIdListPtr->data)) {    // is there data?
#ifdef NEVER
                        if (DebugLevel >= DEBUG_DETAIL) {
                            fprintf(DebugFP,
                                    "%s(): sensor \'%c\' id \"%s\" subId \"%s\" has no sensor records!\n",
                                    __func__, sensor, sensorIdListPtr->id,
                                    sensorSubIdListPtr->subId);
                        }
#endif

                        sensorSubIdListPtr = sensorSubIdListPtr->next;

                        continue;   // no actual sensor data record for now
                    }

                    switch (query) {

                    case QUERY_TYPE_LATEST:
                        
                        // pickup the 1st record (latest)
                        RetIdPtrs[RetCount]    = sensorIdListPtr->id;
                        RetSubIdPtrs[RetCount] = sensorSubIdListPtr->subId;

                        for (i = 0 ; i < MAX_SENSOR_VALUES ; i++) {
                            RetValPtrs[RetCount][i] = &sensorPtr->rawData[i];
                        }

                        RetCount++;

                        break;

                    case QUERY_TYPE_EARLIEST:
                       
                        // pickup last record (earliest)
                        while (sensorPtr->next) {
                            sensorPtr = sensorPtr->next;
                        }

                        RetIdPtrs[RetCount]    = sensorIdListPtr->id;
                        RetSubIdPtrs[RetCount] = sensorSubIdListPtr->subId;

                        for (i = 0 ; i < MAX_SENSOR_VALUES ; i++) {
                            RetValPtrs[RetCount][i] = &sensorPtr->rawData[i];
                        }

                        RetCount++;

                        break;

                    case QUERY_TYPE_ALL:

                        // save them all with each sensor reading being added
                        // to the RetCount
                        while (sensorPtr) {

                            RetIdPtrs[RetCount]    = sensorIdListPtr->id;
                            RetSubIdPtrs[RetCount] = sensorSubIdListPtr->subId;

                            for (i = 0 ; i < MAX_SENSOR_VALUES ; i++) {
                                RetValPtrs[RetCount][i] = &sensorPtr->rawData[i];
                            }

                            RetCount++;

                            sensorPtr = sensorPtr->next;
                        }
                        break;

                    case QUERY_TYPE_AVG:

                        // save them all with each sensor reading being added
                        // to the RetAvgSum for averaging
                        //
                        // we keep track of the number of sensor readings for
                        // each sensor in RetAvgCounts while RetCount is the
                        // number of individual sensors that we're returning
                        // an average for (we could get averages for several
                        // sensors depending on what we ask for
                        RetIdPtrs[RetCount]    = sensorIdListPtr->id;
                        RetSubIdPtrs[RetCount] = sensorSubIdListPtr->subId;

                        for (i = 0 ; i < MAX_SENSOR_VALUES ; i++) {
                            RetAvgSums[RetCount][i] = 0;    // clear this sensor
                        }
                        RetAvgCounts[RetCount] = 0;

                        while (sensorPtr) {
                            for (i = 0 ; i < MAX_SENSOR_VALUES ; i++) {
                                RetAvgSums[RetCount][i] += sensorPtr->rawData[i];
                            }

                            RetAvgCounts[RetCount]++;

                            sensorPtr = sensorPtr->next;
                        }

                        RetCount++;

                        break;
                    }
                }

                sensorSubIdListPtr = sensorSubIdListPtr->next;
            }

        }

        sensorIdListPtr = sensorIdListPtr->next;
    }

#warning  remove strlen()s from each sprintf() call and use return value to run the offset through retBuff

    // format and return the output
 
    // first line of output same for all - leave counts and records for later
    sprintf(retBuff, "\"%s\" %c %c ", tag, query, sensor);

    // formatting depends on query type
    switch (query) {

    case QUERY_TYPE_AVG:

        // average response is complicated - RetCount is the number
        // of sensors that we're returning averages for so handle that first
        //
        sprintf(retBuff + strlen(retBuff), "%d\n", RetCount);
        if (RetCount == 0) {    // returned the 0 so we're done
            break;
        }

        // since there is something to return, return the averages for each
        // sensor found on it's own line (for each potential value)

        for (i = 0; i < RetCount; i++) {
            sprintf(retBuff + strlen(retBuff), "\"%s\" \"%s\"",
                    RetIdPtrs[i], RetSubIdPtrs[i]);

            for (ii = 0; ii < MAX_SENSOR_VALUES ; ii++) {
                sprintf(retBuff + strlen(retBuff), " %d",
                        RetAvgCounts[i] ? RetAvgSums[i][ii] / RetAvgCounts[i] : 0);
            }

            strcat(retBuff, "\n");
        }

        break;

    case QUERY_TYPE_LATEST:
    case QUERY_TYPE_EARLIEST:
    case QUERY_TYPE_ALL:
#warning        make sure dont exceed return buffer
        // could be many records...
        //
        //  * all the records for a specific sensor
        //  * records for many sensors if id or subid wildcards are used
        sprintf(retBuff + strlen(retBuff), "%d\n", RetCount);
        
        for (i = 0 ; i < RetCount ; i++) {
            sprintf(retBuff + strlen(retBuff), "\"%s\" \"%s\"",
                    RetIdPtrs[i], RetSubIdPtrs[i]);

            for (ii = 0; ii < MAX_SENSOR_VALUES ; ii++) {
                sprintf(retBuff + strlen(retBuff), " %d", *RetValPtrs[i][ii]);
            }

            strcat(retBuff, "\n");
        }
        break;
    }

    UNLOCK_SENSOR_LIST;
}



// *****************************************************************
//
// dump routines
//
//  not dependent on DebugLevel - assume they are called based on it
//  so once they are called, they just do output
// *****************************************************************


#define DUMP_LIST_CONTENTS      0
#define DUMP_LIST_STATS         1


static void
dumpSensorList(SENSOR_LIST *ptr)
{
    fprintf(DebugFP, "SENSOR_LIST(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\tType:\t\t\'%c\'\n", ptr->type);
    fprintf(DebugFP, "\tSensors:\t0x%lx\n", (long)ptr->sensors);
    fprintf(DebugFP, "\tNext:\t\t0x%lx\n\n", (long)ptr->next);
}


static void
dumpSensorIdList(SENSOR_ID_LIST *ptr)
{
    fprintf(DebugFP, "\tSENSOR_ID_LIST(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\t\tID:\t\t\"%s\"\n", ptr->id);
    fprintf(DebugFP, "\t\tSubIDs:\t0x%lx\n", (long)ptr->subIds);
    fprintf(DebugFP, "\t\tNext:\t0x%lx\n\n", (long)ptr->next);
}


static void
dumpSensorSubIdList(SENSOR_SUBID_LIST *ptr)
{
    fprintf(DebugFP, "\t\tSENSOR_SUBID_LIST(@0x%lx):\n", (long)ptr);
    fprintf(DebugFP, "\t\t\tSubID:\t\"%s\"\n", ptr->subId);
    fprintf(DebugFP, "\t\t\tData:\t0x%lx\n", (long)ptr->data);
    fprintf(DebugFP, "\t\t\tNext:\t0x%lx\n\n", (long)ptr->next);
}


static void
dumpSensorRecord(SENSOR_RECORD *ptr)
{
    fprintf(DebugFP, "\t\t\tSENSOR_RECORD(@0x%lx):\n", (long)ptr);
#ifdef	__APPLE__
    fprintf(DebugFP, "\t\t\t\tTime:\t%ld.%06d\n\n", ptr->time.tv_sec, ptr->time.tv_usec);
#else
    fprintf(DebugFP, "\t\t\t\tTime:\t%ld.%06ld\n\n", ptr->time.tv_sec, ptr->time.tv_usec);
#endif
    fprintf(DebugFP, "\t\t\t\tType:\t\'%c\' (%d) ", ptr->type, ptr->type);

    switch (ptr->type) {

        case SENSOR_CAMERA:
           fprintf(DebugFP, "[Camera]\n");
           fprintf(DebugFP, "\t\t\t\t\tRaw data - X, Y, W, H:\t%d, %d, %d, %d\n",
                   ptr->rawData[CAMERA_X], ptr->rawData[CAMERA_Y],
                   ptr->rawData[CAMERA_W], ptr->rawData[CAMERA_H]);
           fprintf(DebugFP, "\t\t\t\t\tFiltered data - X, Y, W, H:\t%d, %d, %d, %d\n",
                   ptr->filteredData[CAMERA_X], ptr->filteredData[CAMERA_Y],
                   ptr->filteredData[CAMERA_W], ptr->filteredData[CAMERA_H]);
           fprintf(DebugFP, "\t\t\t\t\tGain - X, Y, W, H:\t%d, %d, %d, %d\n",
                   ptr->gain[CAMERA_X], ptr->gain[CAMERA_Y],
                   ptr->gain[CAMERA_W], ptr->gain[CAMERA_H]);
           break;

        case SENSOR_RANGE:
           fprintf(DebugFP, "[Range]\n");
           fprintf(DebugFP, "\t\t\t\t\tRaw data - Range:\t%d\n",
                   ptr->rawData[RANGE_X]);
           fprintf(DebugFP, "\t\t\t\t\tFiltered data - Range:\t%d\n",
                   ptr->filteredData[RANGE_X]);
           fprintf(DebugFP, "\t\t\t\t\tGain - Range:\t%d\n",
                   ptr->gain[RANGE_X]);
           break;

        case SENSOR_ACCELL:
           fprintf(DebugFP, "[Acceleration]\n");
           fprintf(DebugFP, "\t\t\t\t\tRaw data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->rawData[ACCELL_X], ptr->rawData[ACCELL_Y],
                   ptr->rawData[ACCELL_Z]);
           fprintf(DebugFP, "\t\t\t\t\tFiltered data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->filteredData[ACCELL_X], ptr->filteredData[ACCELL_Y],
                   ptr->filteredData[ACCELL_Z]);
           fprintf(DebugFP, "\t\t\t\t\tGain - X, Y, Z:\t%d, %d, %d\n",
                   ptr->gain[ACCELL_X], ptr->gain[ACCELL_Y],
                   ptr->gain[ACCELL_Z]);
           break;

        case SENSOR_ROLL:
           fprintf(DebugFP, "[Roll]\n");
           fprintf(DebugFP, "\t\t\t\t\tRaw data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->rawData[ROLL_X], ptr->rawData[ROLL_Y],
                   ptr->rawData[ROLL_Z]);
           fprintf(DebugFP, "\t\t\t\t\tFiltered data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->filteredData[ROLL_X], ptr->filteredData[ROLL_Y],
                   ptr->filteredData[ROLL_Z]);
           fprintf(DebugFP, "\t\t\t\t\tGain - X, Y, Z:\t%d, %d, %d\n",
                   ptr->gain[ROLL_X], ptr->gain[ROLL_Y],
                   ptr->gain[ROLL_Z]);
           break;

        case SENSOR_MAGNETIC:
           fprintf(DebugFP, "[Magnetic]\n");
           fprintf(DebugFP, "\t\t\t\t\tRaw data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->rawData[MAGNETIC_X], ptr->rawData[MAGNETIC_Y],
                   ptr->rawData[MAGNETIC_Z]);
           fprintf(DebugFP, "\t\t\t\t\tRaw data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->filteredData[MAGNETIC_X], ptr->filteredData[MAGNETIC_Y],
                   ptr->filteredData[MAGNETIC_Z]);
           fprintf(DebugFP, "\t\t\t\t\tRaw data - X, Y, Z:\t%d, %d, %d\n",
                   ptr->gain[MAGNETIC_X], ptr->gain[MAGNETIC_Y],
                   ptr->gain[MAGNETIC_Z]);
           break;
    }

    fprintf(DebugFP, "\n\t\t\t\tNext:\t0x%lx\n\n", (long)ptr->next);
}


// *****************************************************************
//
// walk routines
//
// *****************************************************************


void
walkSensorRecords(SENSOR_RECORD *ptr, int flag)
{
    int count = 0;

    while (ptr) {
        count++;

        if (flag == DUMP_LIST_CONTENTS) {
            dumpSensorRecord(ptr);
        }
        
        ptr = ptr->next;
    }

    if (flag == DUMP_LIST_STATS) {
        fprintf(DebugFP, "\t\t\t%d SENSOR_RECORD entries\n\n", count);
    }
}



void
walkSensorSubIdList(SENSOR_SUBID_LIST *ptr, int flag)
{
    int count = 0;

    while (ptr) {
        count++;

        if (flag == DUMP_LIST_CONTENTS) {
            dumpSensorSubIdList(ptr);
        }
        
        walkSensorRecords(ptr->data, flag);

        ptr = ptr->next;
    }

    if (flag == DUMP_LIST_STATS) {
        fprintf(DebugFP, "\t\t%d SENSOR_SUBID_LIST entries\n\n", count);
    }
}



void
walkSensorIdList(SENSOR_ID_LIST *ptr, int flag)
{
    int count = 0;

    while (ptr) {
        if (flag == DUMP_LIST_CONTENTS) {
            dumpSensorIdList(ptr);
        }
        
        walkSensorSubIdList(ptr->subIds, flag);

        ptr = ptr->next;
    }

    if (flag == DUMP_LIST_STATS) {
        fprintf(DebugFP, "\t%d SENSOR_ID_LIST entries\n\n", count);
    }
}


void
walkSensorList(SENSOR_LIST *ptr, int flag)
{
    while (ptr) {
        dumpSensorList(ptr);
        
        walkSensorIdList(ptr->sensors, flag);

        ptr = ptr->next;
    }
}


void
dumpListAction(int flag)
{
    SENSOR_LIST         *sensorListPtr;
    struct timeval      now;

    fprintf(DebugFP, "%s:\nVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV\n",
            flag == DUMP_LIST_STATS ? "dumpListStats" : "dumpListRecorfds");

    gettimeofday(&now, NULL);

#ifdef  __APPLE__
    fprintf(DebugFP, "Started @ %ld.%06d\n", now.tv_sec, now.tv_usec);
#else   // ! __APPLE__
    fprintf(DebugFP, "Started @ %ld.%06ld\n", now.tv_sec, now.tv_usec);
#endif  // __APPLE__

    LOCK_SENSOR_LIST;

    if ( ! (sensorListPtr = sensorListGetHead())) {
        fprintf(DebugFP, "\nNO RECORDS\n\n");
        return;
    } else {
        walkSensorList(sensorListPtr, flag);
    }

    UNLOCK_SENSOR_LIST;

    fprintf(DebugFP, "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
}


void
dumpLists()
{
    dumpListAction(DUMP_LIST_CONTENTS);
}


void
dumpListStats()
{
    dumpListAction(DUMP_LIST_STATS);
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
    int i;

    ptr->time.tv_sec   = 0;
    ptr->time.tv_usec  = 0;
 
    for (i = 0 ; i < MAX_SENSOR_VALUES ; i++) {
        ptr->rawData[i] = ptr->filteredData[i] = ptr->gain[i] = 0;
    }

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


/// \brief frees all sensor records starting at some location in the list
///
/// records are freed from the end of the list popping back towards the first
/// on to be purged
///
/// doesn't lock list - assumes caller did


static void
freeSensorRecsFromEnd(SENSOR_RECORD *ptr)
{

    if (DebugLevel >= DEBUG_SUPER) {
        fprintf(DebugFP, "%s(0x%lx) entered\n", __func__, (long)ptr);
    }

    if (ptr->next) {    // keep going
        freeSensorRecsFromEnd(ptr->next);  // RECURSIVE!!!
    }

    cvFree(ptr);    // free ME!
}


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
freeSensorRecord(SENSOR_RECORD *ptr)
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
static void
getLock(pthread_mutex_t *lock)
{
    int retVal;

    if ((retVal = pthread_mutex_lock(lock)) == 0) {    // locked
        return;
    }
#ifdef  LOCK_DEBUG	// BE CAREFUL!  LOCK_DEBUG adds 'else' to 'if' above!!
      else {

        char    *errorString;

        errorString = strerror(errno);

        if (DebugLevel >= DEBUG_SUPER) {
            fprintf(DebugFP, "%s(0x%lx): pthread_mutex_lock() returned %d (%s)\n", __func__,
                    (long)lock, retVal, (retVal != 0) ? errorString : "Success");
        }
    }
#endif  // LOCK_DEBUG

    exit(1);
}



/// \brief release the specified lock
///
/// returns 0 if successful - non-zero if unsuccessful
///
static void
releaseLock(pthread_mutex_t *lock)
{
    if (pthread_mutex_unlock(lock) != 0) {

#ifdef  LOCK_DEBUG
        char    *errorString;

        errorString = strerror(errno);

        if (DebugLevel >= DEBUG_SUPER && retVal) {
            fprintf(DebugFP, "%s(0x%lx): pthread_mutex_unlock() returned %d (%s)\n", __func__,
                    (long)lock, retVal, (retVal != 0) ? errorString : "Success");
        }
#endif  // LOCK_DEBUG

        exit(1);
    }
}
