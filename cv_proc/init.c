//    cv_proc_init.c
//
//    vision processor initialization

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

#include "cv.h"
#include "cv_net.h"
#include "cv_cam.h"
#include "cv_externs.h"




static  void        usage(), openIncomingPort(HOST_INFO *),
                    dumpConfig();

static  CAM_RECORD  *initCameraRecords(int);



void
cvProcInit(int argc, char **argv)

{
    int c;

    // clear HostInfo structure
    HostInfo.hostIPString = DEF_HOST_IP_STRING;
    HostInfo.hostPort     = DEF_HOST_PORT;

    while ((c = getopt(argc, argv, "h:p:D:")) != -1) {

        switch (c) {

        case 'h':
            HostInfo.hostIPString = optarg;
            if (inet_pton(AF_INET, HostInfo.hostIPString, &(HostInfo.hostIP.sin_addr.s_addr)) != 1) {
                fprintf(stderr, "%s: error: invalid IP address value (\"%s\")\n",
                        MyName, HostInfo.hostIPString);
                exit(1);
            }
            break;

        case 'p':
            HostInfo.hostPort = atoi(optarg);
            if (HostInfo.hostPort < 1 || HostInfo.hostPort > (64*1024 - 1)) {
                fprintf(stderr, "%s: error: invalid port value (%d)\n",
                        MyName, HostInfo.hostPort);
                exit(1);
            }
            break;

        case 'D':
            DebugLevel = atoi(optarg);
            
            switch (DebugLevel) {
                
            case DEBUG_OFF:
            case DEBUG_INFO:
            case DEBUG_DETAIL:
                break;  // all valid values
            default:
                fprintf(stderr, "%s: error: invalid debug value (%d)\n",
                        MyName, DebugLevel);
                usage();
                exit(1);
                break;
            }
            break;

        case '?':
            usage();
            exit(1);
            break;

       default:
            printf("?? getopt returned character code 0x%x ??\n", c);
        }
    }

    Cameras[CAM_LEFT]  = initCameraRecords(MAX_CAMERA_RECORDS);
    Cameras[CAM_RIGHT] = initCameraRecords(MAX_CAMERA_RECORDS);

    openIncomingPort(&HostInfo);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }

}



static CAM_RECORD *
initCameraRecords(int count)
{
    int         i;
    CAM_RECORD  *set, *ptr;

    if ((set = (CAM_RECORD *)malloc(count * sizeof(CAM_RECORD))) == (CAM_RECORD *)NULL) {
        fprintf(stderr, "%s: error: malloc() failure on initializing camera structures\n",
                MyName);
        exit(1);
    }

    ptr = set;

    for (i = 0 ; i < count ; i++) {

        ptr->secs  = 0;
        ptr->usecs = 0;

        ptr->id[0]     = '\0';
        ptr->x         = 0;
        ptr->y         = 0;
        ptr->w         = 0;
        ptr->h         = 0;

        ptr++;
    }

    return set;
}




static void
dumpConfig()
{
    printf("DumpConfig():\n");
    printf("Network:\n\tListening @:\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);

    printf("\nDebug:\t%d ", DebugLevel);

    switch (DebugLevel) {
    case 0:
        printf("(OFF)\n");
        break;

    case 1:
        printf("(INFO)\n");
        break;

    case 2:
        printf("(DETAIL)\n");
        break;
    }

    printf("--------------------------------------\n\n");
}




static void
openIncomingPort(HOST_INFO *host)
{
    if ((host->sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {      // UDP
        fprintf(stderr, "%s: error: cannot open socket (%s)\n",
                MyName, strerror(errno));
        exit(1);
    }

    memset(&(host->hostIP), 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, host->hostIPString, &(host->hostIP.sin_addr.s_addr));
    host->hostIP.sin_port   = htons(host->hostPort);
    host->hostIP.sin_family = AF_INET; // Use IPv4

    if (bind(host->sock, (struct sockaddr *)&(host->hostIP),
             sizeof(struct sockaddr_in)) == -1) {
        fprintf(stderr, "%s: error: cannot bind socket (%s)\n",
                MyName, strerror(errno));
        exit(1);
    }
}




static void
usage()

{
    fprintf(stderr,
            "%s: usage: %s -h Host IP -p Host Port [-D 0|1|2]\n",
            MyName, MyName);
}
