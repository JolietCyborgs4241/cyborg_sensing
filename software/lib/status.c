//  status.c   cyborg autonomous status display update routines
//
//  intended to be called from programs to update their status
//
//      - send status to status server (non-blocking)
//      - status server talks to status bar driver hardware
//        - Arduino-based
//        - Communicates over serial protocol


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
#include "status/status.h"


static  HOST_INFO   localHostInfoCopy;


void    *cvAlloc();


/// \brief open a status connection
///
/// allocates space for a local copy of the ASCII host address
void
openStatusConnection(HOST_INFO *host)
{
    openOutgoingPort(host);

    localHostInfoCopy = *host;

    localHostInfoCopy.hostIPString = cvAlloc(strlen(host->hostIPString) + 1);
    strcpy(localHostInfoCopy.hostIPString, host->hostIPString);
}





/// \brief  send an update for a single led to the status display server
///
/// led to set
/// rgb value to
int
sendStatusUpdate(unsigned char led, unsigned char color)
{
    int sockRet;
    STATUS_MSG  status;

    status.led   = led;
    status.color = color;

    if ((sockRet = sendto(localHostInfoCopy.sock, &status, sizeof(status), 0,
                          (struct sockaddr *)&localHostInfoCopy.hostIP,
                          sizeof(struct sockaddr_in))) == -1) {

        return 0;           // error !
    }

    return sockRet;         // succcess
}

