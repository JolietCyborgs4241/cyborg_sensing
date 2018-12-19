//  network.c   cyborg autonomous network routines

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "cv_net.h"




/// \brief Open UDP port for sending messages 
///
/// Create socket
///
/// Socket is set in HOST_INFO structure
///
/// assumes address information is set in HOST_INFO structure passed to routine
void
openOutgoingPort(HOST_INFO *host)
{
    if ((host->sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {      // UDP
        fprintf(stderr, "%s(): error: cannot open socket (%s)\n",
                __func__, strerror(errno));
        exit(1);
    }

    memset(&(host->hostIP), 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, host->hostIPString, &(host->hostIP.sin_addr.s_addr));
    host->hostIP.sin_port   = htons(host->hostPort);
    host->hostIP.sin_family = AF_INET; // Use IPv4
}




/// \brief Open UDP port for receiving messages
///
/// Create socket
///
/// Bind to appropriate address
///
/// Assumes HOST_INFO structure is set appropriately
///
/// Socket is set in HOST_INFO structure
void
openIncomingPort(HOST_INFO *host)
{
    if ((host->sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {      // UDP
        fprintf(stderr, "%s(): error: cannot open socket (%s)\n",
                __func__, strerror(errno));
        exit(1);
    }

    memset(&(host->hostIP), 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, host->hostIPString, &(host->hostIP.sin_addr.s_addr));
    host->hostIP.sin_port   = htons(host->hostPort);
    host->hostIP.sin_family = AF_INET; // Use IPv4

    if (bind(host->sock, (struct sockaddr *)&(host->hostIP),
             sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "%s(): error: cannot bind socket (%s)\n",
                __func__, strerror(errno));
        exit(1);
    }
}

