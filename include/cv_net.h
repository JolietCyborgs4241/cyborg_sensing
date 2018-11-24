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




#define DEF_HOST_IP_STRING  "127.0.0.1"
#define DEF_HOST_PORT       12345

typedef struct {
    char    *hostIPString;
    struct  sockaddr_in hostIP;
    int     hostPort;
    int     sock;
} HOST_INFO;



#endif  /* cn_net.h */
