//    9dof init.c
//
//    server for 9 degrees of freedom sensor
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
#include "sensors.h"
#include "9dof/externs.h"




static  void        usage(), dumpConfig();




/// \brief process command line and perform other initializations
///
/// -h Host IP:port of database
///
/// -S Host IP: of status server
///
/// -s serial port@speed to read from
///
/// -I Sensor ID
///
/// -i Sensort SubID
///
/// -d Debug output file
///
/// -D Debug level
///
/// -l Log output file
///
/// Also opens UDP socket for sending messages to database and status server
void
init(int argc, char **argv)

{
    int c, ttl;

    HostInfo.hostIPString     = DEF_HOST_IP_STRING;
    HostInfo.hostPort         = DEF_HOST_PORT_DB_POST;

    StatusServer.hostIPString = DEF_HOST_IP_STRING;
    StatusServer.hostPort     = DEF_HOST_PORT_STATUS;

    while ((c = getopt(argc, argv, "h:S:s:D:d:l:I:i:")) != -1) {

        switch (c) {

        case 'h':
            setHostAndPort(optarg, &HostInfo);
            break;

        case 'S':
            setHostAndPort(optarg, &StatusServer);
            break;

        case 'I':
            SensorId = optarg;
            break;

        case 'i':
            SensorSubId = optarg;
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

    if (SerialPort == NULL) {
        fprintf(stderr, "%s: error: serial port (-s) must be specified\n",
                MyName);
        exit(1);
    }

    if (strlen(SensorId) == 0) {
        fprintf(stderr, "%s: error: SensorID (-I) must be specified\n",
                MyName);
        exit(1);
    }
    openOutgoingPort(&HostInfo);

    SerialFd = openSerialPort(SerialPort);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    fprintf(DebugFP, "DumpConfig():\n");
    fprintf(DebugFP, "Network:\n\tDatabase @:\t\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);
    fprintf(DebugFP, "\t\tStatus Server @:\t%s:%d\n\tSock fd:\t%d\n",
           StatusServer.hostIPString, StatusServer.hostPort,
           StatusServer.sock);

    fprintf(DebugFP, "Serial:\t\"%s\"\n", SerialPort);

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
/// Shown is any invalid parameters specvified or '-?' on command line
static void
usage()

{
    fprintf(stderr,
            "%s: usage: %s [-h IP:port] [-S IP:Port] [-I ID] [-i SubID] [-s 9dof comm port@speed] [-d debug file] [-D 0|1|2|3] [ -l log file ]\n",
            MyName, MyName);
}
