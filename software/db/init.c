//    cv_db_init.c
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
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "sensors.h"
#include "db/externs.h"
#include "status/status.h"




static  void        usage(), dumpConfig();


/// \brief process command line and perform other initializations
///
/// -S Host IP and port for status server
///
/// -t TTL (seconds - 0 disables any purging)
///
/// -d Debug output file
///
/// -D Debug level
///
void
init(int argc, char **argv)

{
    int c, ttl;

    gettimeofday(&StartTime, NULL);

    HostInfoPost.hostIPString  = DEF_HOST_IP_STRING;
    HostInfoPost.hostPort      = DEF_HOST_PORT_DB_POST;

    HostInfoQuery.hostIPString = DEF_HOST_IP_STRING;
    HostInfoQuery.hostPort     = DEF_HOST_PORT_DB_QUERY;

    StatusServer.hostIPString  = DEF_HOST_IP_STRING;
    StatusServer.hostPort      = DEF_HOST_PORT_STATUS;


    while ((c = getopt(argc, argv, "S:P:Q:D:d:t:l:")) != -1) {

        switch (c) {

        case 'S':
            setHostAndPort(optarg, &StatusServer);
            break;

        case 'P':
            setHostAndPort(optarg, &HostInfoPost);
            break;

        case 'Q':
            setHostAndPort(optarg, &HostInfoQuery);
            break;

        case 't':
            ttl = atoi(optarg);
            if (ttl < 0) {
                fprintf(stderr, "%s: error: TTL value must be >= 0\n",
                        MyName);
                exit(1);
            }

            // set all sensor TTLs to the same for now
            TTLS    *ttlPtr = SensorTtls;

            while (ttlPtr->sensor) {
                ttlPtr->ttlSecs  = ttl;
                ttlPtr->ttlUsecs = 0;
#warning Need to add floating point value TTL support
                ttlPtr++;
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

    initMutexes();

    initDb();

    openIncomingPort(&HostInfoPost);

    openIncomingPort(&HostInfoQuery);

    openStatusConnection(&StatusServer);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    fprintf(DebugFP, "DumpConfig():\n");
    fprintf(DebugFP, "Network:\n\tPost @:\t\t\t%s:%d\n\tSock fd:\t\t%d\n",
           HostInfoPost.hostIPString, HostInfoPost.hostPort,
           HostInfoPost.sock);
    fprintf(DebugFP, "Network:\n\tQuery @:\t\t%s:%d\n\tSock fd:\t\t%d\n",
           HostInfoQuery.hostIPString, HostInfoQuery.hostPort,
           HostInfoQuery.sock);
    fprintf(DebugFP, "Network:\n\tStatus Server @:\t%s:%d\n\tSock fd:\t\t%d\n",
           StatusServer.hostIPString, StatusServer.hostPort,
           StatusServer.sock);

    fprintf(DebugFP, "\nTTLs:\n");

    TTLS *ttlPtr = SensorTtls;

    while (ttlPtr->sensor) {
        fprintf(DebugFP, "\tSensor \'%c\':\t%d.%06d\n",
                ttlPtr->sensor, ttlPtr->ttlSecs, ttlPtr->ttlUsecs);
        ttlPtr++;
    }

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
            "%s: usage: %s [-P IP:port] [-Q IP:port] [-S IP:port] [-t ttl] [-d debug file] [-D 0|1|2|3] [-l log file]\n",
            MyName, MyName);
}
