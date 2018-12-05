//  sensors.c
//
//  sensor-specific  handling for cv_db

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "db/lists.h"
#include "db/externs.h"
#include "sensors.h"






void
processCamData(char *buffer)
{
    char    id[MAX_SENSOR_READ], camera;
    int     scanRet, x, y, w, h;

    // validate and set camera
    // should be "[RL] N2 " at head
    if (*buffer != SENSOR_CAM || *(buffer + 1) != ' ') {
        fprintf(DebugFP, "%s: error: %s() sensor header error\n", MyName, __func__);
        return;
    }

    buffer += 2;           // Starts with "C " so continue

    if (*buffer == CAMERA_LEFT_ID || *buffer == CAMERA_RIGHT_ID) {
        camera = *buffer;
    } else {
        fprintf(DebugFP, "%s: error: unknown camera '%c'\n", MyName, *buffer);
        return;
    }

    buffer++;

    if (*buffer != ' ') {
        fprintf(DebugFP, "%s: error: non-space 2nd char '%c'\n", MyName, *(buffer + 1));
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

    // seems OK start scan after first 2 chars (camera, space, type, space)
    scanRet = sscanf(buffer, "%s %d %d %d %d",
                     id, &x, &y, &w, &h);

    if (scanRet != 5) {         // scanning error
        fprintf(DebugFP, "%s(): scanf(\"%s\") error: ret %d\n", __func__, buffer, scanRet);
        return;
    }

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): validated - adding msg\n", __func__);
    }
    camRecAdd(id, camera, x, y, w, h);
}





void
processRangerData(char *buffer)
{
    char    ranger;
    int     dist;

    if (*buffer != SENSOR_RANGE || *(buffer + 1) != ' ') {
        fprintf(DebugFP, "%s: error: %s() sensor header error (\"%s\")\n",
                MyName, __func__, buffer);
        return;
    }

    buffer += 2;            // starts with "R " so continue

    ranger = *buffer;       // get id char

    buffer++;

    if (*buffer != ' ') {
        fprintf(DebugFP, "%s: error: non-space after id char '%c'\n", MyName, *buffer);
        return;
    }

    buffer++;               // now point to the real data value

    dist = atoi(buffer);

    if (dist <= 0) {        // scanning error
        fprintf(DebugFP, "%s(): atoi(\"%s\") error: ret %d\n", __func__, buffer, dist);
        return;
    }

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): validated - adding msg\n", __func__);
    }

    sensorRecAdd(SENSOR_RANGE, ranger, dist, 0, 0, 0);  // unused params are 0
}


// all the different readings form the 9D sensor (G, roll, mag) look the same
// so we can process and validate them all together
void
process9DData(char *buffer)
{
    char    id, type;
    int     scanRet, x, y, z;

    if (((*buffer != SENSOR_G) && (*buffer != SENSOR_ROLL) && (*buffer != SENSOR_MAG))
       ||  *(buffer + 1) != ' ') {
        fprintf(DebugFP, "%s: error: %s() sensor header error (\"%s\")\n",
                MyName, __func__, buffer);
        return;
    }

    buffer += 2;            // starts with "[GOM] " so continue

    id = *buffer;       // get id char

    buffer++;

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

    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(): validated - adding msg\n", __func__);
    }

    sensorRecAdd(type, id, x, y, z, 0);  // unused params are 0
}
