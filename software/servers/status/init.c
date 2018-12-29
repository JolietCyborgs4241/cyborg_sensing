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
#include "status/status.h"




static  void        usage(), dumpConfig();



/// \brief process command line and perform other initializations
///
/// -S IP:port to listen at
///
/// -s serial port @ speed to connect to status display hardware
///
/// -t LED TTL (seconds)
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

    // set HostInfo structure
    HostInfo.hostIPString = DEF_HOST_IP_STRING;
    HostInfo.hostPort     = DEF_HOST_PORT_STATUS;  // port to listen at

    while ((c = getopt(argc, argv, "S:s:D:d:t:l:")) != -1) {

        switch (c) {

        case 'S':
            setHostAndPort(optarg, &HostInfo);
            break;

        case 's':
            SerialPort = optarg;
            break;

        case 't':
            LedTtl = atoi(optarg);
            if (LedTtl < 1 || LedTtl > MAX_LED_TTL) {
                fprintf(stderr, "%s: error: invalid LED TTL value \"%s\" - must be between 1 and %d seconds\n",
                        MyName, optarg, MAX_LED_TTL);
                exit(1);
            }
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
    fprintf(DebugFP, "Network:\n\tListening at:\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);

    fprintf(DebugFP, "Serial connection:\t\"%s\"\n", SerialPort);

    fprintf(DebugFP, "LED TTL:\t\t%d\n", LedTtl);

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




/// \brief Usage message
///
/// Shown is any invalid parameters specified or '-?' on command line
static void
usage()

{
    fprintf(stderr,
            "%s: usage: %s [-S host:port] [-s serial_port@speed ] [-t LED TTL ] [-d debug file ] [-D 0|1|2|3] [-l log file]\n",
            MyName, MyName);
}

