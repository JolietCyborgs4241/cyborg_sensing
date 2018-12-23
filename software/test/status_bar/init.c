//    status_bar init.c
//
//    test driver for LED status bar
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



extern  int     MaxDelay, RandomCount;

static  void    usage(), dumpConfig();



/// \brief process command line and perform other initializations
///
/// -s serial port @ speed to connect to status display hardwaee
///
/// -t LED timeout (sec)
///
/// -M  max inter-LED random delay (msec)
///
/// -R random sequence count
//
/// -d Debug output file
///
/// -D Debug level
///
/// -L Log output file
///
void
init(int argc, char **argv)

{
    int c, ttl;

    while ((c = getopt(argc, argv, "s:D:d:l:t:R:M:")) != -1) {

        switch (c) {

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

        case 'M':
            MaxDelay = atoi(optarg);
            if (MaxDelay < 1) {
                fprintf(stderr, "%s: error: invalid max delay value \"%s\"\n",
                        MyName, optarg);
                exit(1);
            }
            break;

        case 'R':
            RandomCount = atoi(optarg);
            if (RandomCount < 1) {
                fprintf(stderr, "%s: error: invalid random count value \"%s\"\n",
                        MyName, optarg);
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

    SerialFd = openSerialPort(SerialPort);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    fprintf(DebugFP, "Serial connection:\t\"%s\"\n", SerialPort);

    fprintf(DebugFP, "\n LED TTL:\t\t%d ", LedTtl);

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
            "%s: usage: %s [-h IP] [-p Port] [-s serial_port@speed ] [ -t LED TTL ] [ -M max inter msg delay (msecs) ] [ -R random cycle count ] [-d debug file ] [-D 0|1|2|3] [-l log file]\n",
            MyName, MyName);
}

