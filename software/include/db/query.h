//  query.h
//
//  structures and function prototypes for querying
//  sensor data records

#ifndef _CYBORG_DB_QUERY_H_
#define _CYBORG_DB_QUERY_H_ 1

#define MAX_QUERY_SIZE      100

/// MAX_QUERY_RESP
///
/// stay under network MTU in case we are not using localhost
/// (local host MTU is 64K while 802.11 is 1500)
#define MAX_QUERY_RESP      1450

#define MAX_QUERY_RET_VALS  20


void processQuery(char *, char, char, char *, char *, char *, int);


/// query:
///
/// query_tag query_type sensor id subid
///
///   query_tag - tag from requester (used to match responses)
///
///   query_type - L (latest), E (earliest), A (all), V (average of all active)
///
///   sensor (sensor type)
///
///   id
///
///   subId

#define QUERY_TYPE_LATEST       'L'
#define QUERY_TYPE_EARLIEST     'E'
#define QUERY_TYPE_ALL          'A'
#define QUERY_TYPE_AVG          'V'


/// query response:
///
/// query_tag query_type sensor count <newline>
///   id subId i1 i2 i3 i4 <newline>
///   ...
///
///   query_tag - tag from requester
///
///   query_type -  see request
///
///   sensor
///
///   count (could be 0)
///
///   per sensor response:
///
///     id subid i1 i2 i3 i4 (unused values returned as 0)
//

#endif  /* query.h */
