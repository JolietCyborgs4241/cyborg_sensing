//	cv_net.h
//
//  general defines for cyborg_vision network stuff
//


#ifndef _CV_NET_H_
#define _CV_NET_H_   1


#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>




#define DEF_HOST_IP_STRING      "127.0.0.1"

/// default port for posting data to cv_db
#define DEF_HOST_PORT_DB_POST   12345
/// default port for querying data from cv_db
#define DEF_HOST_PORT_DB_QUERY  12346
/// default port for querying data from cv_db
#define DEF_HOST_PORT_STATUS    12347

/// used for storing host addressing information
typedef struct {
    /// host interface to listen for incoming UDP packets from cameras
    char    *hostIPString;
    struct  sockaddr_in hostIP;
    /// host port to listen for incoming UDP packets from cameras
    int     hostPort;
    /// socket to recieve camera packets on
    int     sock;
} HOST_INFO;



#endif  /* cv_net.h */
