//  query.h
//
//  structures and function prototypes for querying
//  sensor data records

#ifndef _CYBORG_DB_QUERY_H_
#define _CYBORG_DB_QUERY_H_ 1

#define MAX_QUERY_REQ_ID_LEN     40

typedef unsigned char   QUERY_TYPE;

typedef struct queryRequest {
    /// Query type
    QUERY_TYPE      query;

    /// Sensor type
    SENSOR_TYPE     type;

    /// Sensor identifying information
    ///
    /// id/subid  "ball/front"  "*/front"  "ball/*"
    char            identifier[MAX_QUERY_REQ_ID_LEN];
} QUERY_REQUEST;


typedef struct sensorVals {
    /// id/sub (repeated for each record)
    char            identifier[MAX_QUERY_REQ_ID_LEN];

    /// Sensor values
    ///
    /// depends on sensor type
    int             i1, i2, i3, i4;
} SENSOR_VALS;


typedef struct queryResponse {
    /// Query type
    QUERY_TYPE      query;

    /// Sensor type
    SENSOR_TYPE     type;

    /// Number of sensor records returned
    /// (could be 0 if none found)
    int             retCount;

    /// Actual returned sensor data (up to UDP MTU)
    SENSOR_VALS     sensorVals[1];
} QUERY_RESPONSE;

#endif  /* query.h */
