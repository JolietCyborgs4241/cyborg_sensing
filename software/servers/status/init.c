//    gen_sss init.c
//
//    server for serial-based sensors that provide already formatted
//    readings ready for insertion into the database
//

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

#include "cv.h"
#include "cv_net.h"
#include "cyborg_lib.h"
#include "status/externs.h"




static  void        usage(), openIncomingPort(HOST_INFO *),
                    dumpConfig();



/// \brief process command line and perform other initializations
///
/// -h Host IP to listen at
///
/// -p Host port to listen at
///
/// -s serial port @ speed to connect to status display hardwaee
///
/// -d Debug output file
///
/// -D Debug level
///
/// -L Log output file
///
/// Also opens UDP socket for listening for incoming status commands
void
init(int argc, char **argv)

{
    int c, ttl;

    // clear HostInfo structure
    HostInfo.hostIPString = DEF_HOST_IP_STRING;
    HostInfo.hostPort     = DEF_HOST_PORT_STATUS;  // port to listen at

    while ((c = getopt(argc, argv, "h:p:s:D:d:t:l:")) != -1) {

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

        case 's':
            SerialPort = optarg;
            break;

        case 'D':
            DebugLevel = atoi(optarg);
            
            switch (DebugLevel) {
                
            case DEBUG_OFF:
            case DEBUG_INFO:
            case DEBUG_DETAIL:
            case DEBUG_SUPER:
                break;  // all valid values
            default:
                fprintf(stderr, "%s: error: invalid debug value (%d)\n",
                        MyName, DebugLevel);
                usage();
                exit(1);
                break;
            }
            break;

        case 'd':
            if ((DebugFP = fopen(optarg, "w")) == (FILE *)NULL) {
                fprintf(stderr, "%s: error: cannot open debug file \"%s\"\n", MyName, optarg);
                exit(1);
            }

            setbuf(DebugFP, (char *)NULL);      // no buffering

            break;

        case 'l':
            if ((LogFP = fopen(optarg, "w")) == (FILE *)NULL) {
                fprintf(stderr, "%s: error: cannot open log file \"%s\"\n", MyName, optarg);
                exit(1);
            }

            setbuf(LogFP, (char *)NULL);      // no buffering

            break;

        case '?':
            usage();
            exit(1);
            break;

        default:
            fprintf(stderr, "?? getopt returned character code 0x%x ??\n", c);
        }
    }

    openIncomingPort(&HostInfo);

    SerialFd = openSerialPort(SerialPort);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    fprintf(DebugFP, "Network:\n\tSending to:\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);

    fprintf(DebugFP, "Serial connection:\t\"%s\"\n", SerialPort);

    fprintf(DebugFP, "\nDebug:\t%d ", DebugLevel);

    switch (DebugLevel) {
    case DEBUG_OFF:
        fprintf(DebugFP, "(OFF)\n");
        break;

    case DEBUG_INFO:
        fprintf(DebugFP, "(INFO)\n");
        break;

    case DEBUG_DETAIL:
        fprintf(DebugFP, "(DETAIL)\n");
        break;

    case DEBUG_SUPER:
        fprintf(DebugFP, "(DETAIL)\n");
        break;
    }

    fprintf(DebugFP, "--------------------------------------\n\n");
}




/// \brief Open UDP port for recieving status updates
///
/// Create socket
///
/// Socket is set in HOST_INFO structure
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



/// \brief Usage message
///
/// Shown is any invalid parameters specvified or '-?' on command line
static void
usage()

{
    fprintf(stderr,
            "%s: usage: %s [-h IP] [-p Port] [-s serial_port@speed ] [-d debug file ] [-D 0|1|2|3] [-l log file]\n",
            MyName, MyName);
}

