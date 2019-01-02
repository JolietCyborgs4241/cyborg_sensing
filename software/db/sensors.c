//  sensors.c
//
//  sensor-specific  handling for cv_db

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>

#include "cv.h"
#include "cv_net.h"
#include "db/lists.h"
#include "db/externs.h"
#include "sensors.h"





void
processCamData(char *buffer)
{
    char    id[MAX_SENSOR_READ], camera[MAX_SENSOR_READ], *camPtr;
    int     scanRet, x, y, w, h;

    // validate and set camera
    // should be "[RL] N2 " at head
    if (*buffer != SENSOR_CAMERA || *(buffer + 1) != ' ') {
        fprintf(DebugFP, "%s: error: %s() sensor header error\n", MyName, __func__);
        return;
    }

    buffer += 2;           // Starts with "C " so continue

    camPtr = camera;

    while ( ! isspace(*buffer)) {
        *camPtr++ = *buffer++;
    }

    if (camPtr == camera) {     // this means there was no camera name
        fprintf(DebugFP, "%s: error: blank camera field\n", MyName);
        return;
    }

    *camPtr = '\0';         // terminate the camera string

    if (*buffer != ' ') {
        fprintf(DebugFP, "%s: error: non-space char '%c' following camera name\n",
                MyName, *buffer);
        return;
    }

    buffer++;

    // should be an "N2 " here to identify the record type from the camera
    if(*buffer != 'N' && *(buffer+1) != '2' && *(buffer+2) != ' ') {
        fprintf(DebugFP, "%s: error: invalid record type \'%c%c\'\n",
                MyName, *(buffer+2), *(buffer+3));
        return;
    }

    buffer += 3;                // now point to the real data values

    // seems OK, start scan
    scanRet = sscanf(buffer, "%s %d %d %d %d",
                     id, &x, &y, &w, &h);

    if (scanRet != 5) {         // scanning error
        fprintf(DebugFP, "%s(): scanf(\"%s\") error: ret %d\n", __func__, buffer, scanRet);
        return;
    }

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s():calling sensorRecAdd(\'%c\', \"%s\", \"%s\", %d, %d, %d, %d)\n",
                __func__, SENSOR_CAMERA, id, camera, x, y, w, h);
    }
    
    sensorRecAdd(SENSOR_CAMERA, id, camera, x, y, w, h);
}





void
processOneValSensor(char *buffer)
{
    char    id[MAX_SENSOR_READ], *ptr, sensor;
    int     value;

    if ((*buffer != SENSOR_RANGE && *buffer != SENSOR_OBSTACLE)
        || *(buffer + 1) != ' ') {
        fprintf(DebugFP, "%s: error: %s() sensor header error (\"%s\")\n",
                MyName, __func__, buffer);
        return;
    }

    sensor = *buffer;

    buffer += 2;            // starts appropriately so continue

    // copy the id and null terminate it
    //
    // make sure we don't fall off the end of 'buffer' or the 'id' buffer and look
    // for a space to terminate the id sting in the sensor record
    //
    // it would be simpler to just point to the id snd add the terminator to the
    // buffer itself but we don't want to alter the sensor record in this routine

    ptr = id;
    while (*buffer && *buffer != ' ' && (ptr - id <= sizeof(id) -1)) {
        *ptr++ = *buffer++;
    }

    if ( ! *buffer) {
        fprintf(DebugFP, "%s: error: %s() early sensor record termination while processing id\n",
                __func__, MyName);
        return;
    }

    *ptr = '\0';

    if (*buffer != ' ') {
        fprintf(DebugFP, "%s: error: non-space after id char '%c'\n", MyName, *buffer);
        return;
    }

    buffer++;               // now point to the real data value

    value = atoi(buffer);

    if (value <= 0) {        // scanning error
        fprintf(DebugFP, "%s(): atoi(\"%s\") error: ret %d\n", __func__, buffer, value);
        return;
    }

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): validated - adding msg\n", __func__);
    }

    sensorRecAdd(sensor, id, "", value, 0, 0, 0);  // unused params are 0
}


// all the different readings form the 9D sensor (G, roll, mag) look the same
// so we can process and validate them all together
void
process9DData(char *buffer)
{
    char    id[MAX_SENSOR_READ], *ptr;
    SENSOR_TYPE   type;
    int     scanRet, x, y, z;

    if (((*buffer != SENSOR_ACCELL) && (*buffer != SENSOR_ROLL)
         && (*buffer != SENSOR_MAGNETIC)) ||  *(buffer + 1) != ' ') {

        fprintf(DebugFP, "%s: error: %s() sensor header error (\"%s\")\n",
                MyName, __func__, buffer);
        return;
    }

    type = (SENSOR_TYPE)*buffer;

    buffer += 2;            // starts with "[GOM] " so continue

    // copy the id and null terminate it
    //
    // make sure we don't fall off the end of 'buffer' or the 'id' buffer and look
    // for a space to terminate the id sting in the sensor record
    //
    // it would be simpler to just point to the id snd add the terminator to the
    // buffer itself but we don't want to alter the sensor record in this routine

    ptr = id;
    while (*buffer && *buffer != ' ' && (ptr - id <= sizeof(id) -1)) {
        *ptr++ = *buffer++;
    }

    if ( ! *buffer) {
        fprintf(DebugFP, "%s: error: %s() early sensor record termination while processing id\n",
                __func__, MyName);
        return;
    }

    *ptr = '\0';

    if (*buffer != ' ') {
        fprintf(DebugFP, "%s: error: non-space after id char '%c'\n", MyName, *buffer);
        return;
    }

    buffer++;               // now point to the real data values

    // seems OK start scan
    scanRet = sscanf(buffer, "%d %d %d",
                     &x, &y, &z);

    if (scanRet != 3) {         // scanning error
        fprintf(DebugFP, "%s(): scanf(\"%s\") error: ret %d\n", __func__, buffer, scanRet);
        return;
    }

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): validated - adding msg\n", __func__);
    }

    sensorRecAdd(type, id, "", x, y, z, 0);  // unused params are 0
}
