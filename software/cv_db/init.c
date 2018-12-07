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

#include "cv.h"
#include "cv_net.h"
#include "db/externs.h"




static  void        usage(), openIncomingPort(HOST_INFO *),
                    dumpConfig();


/// \brief process command line and perform other initializations
///
/// -h Host IP
///
/// -p Host port
///
/// -t TTL (seconds - 0 disables any purging)
///
/// -d Debug output file
///
/// -D Debug level
///
/// Also opens UDP socket for receiving camera messages
void
init(int argc, char **argv)

{
    int c;

    // clear HostInfo structure
    HostInfo.hostIPString = DEF_HOST_IP_STRING;
    HostInfo.hostPort     = DEF_HOST_PORT_DB_POST;  // port to send to add data

    while ((c = getopt(argc, argv, "h:p:D:d:t:")) != -1) {

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

        case 't':
            Ttl = atoi(optarg);
            if (Ttl < 0) {
                fprintf(stderr, "%s: error: TTL value must be >= 0\n",
                        MyName);
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

        case 'd':
            if ((DebugFP = fopen(optarg, "w")) == (FILE *)NULL) {
                fprintf(stderr, "%s: error: cannot open debug file \"%s\"\n", MyName, optarg);
                exit(1);
            }

            setbuf(DebugFP, (char *)NULL);

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

    openIncomingPort(&HostInfo);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    fprintf(DebugFP, "DumpConfig():\n");
    fprintf(DebugFP, "Network:\n\tListening @:\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);

    fprintf(DebugFP, "\nTTL:\t%d ", Ttl);

    fprintf(DebugFP, "\nDebug:\t%d ", DebugLevel);

    switch (DebugLevel) {
    case 0:
        fprintf(DebugFP, "(OFF)\n");
        break;

    case 1:
        fprintf(DebugFP, "(INFO)\n");
        break;

    case 2:
        fprintf(DebugFP, "(DETAIL)\n");
        break;
    }

    fprintf(DebugFP, "--------------------------------------\n\n");
}



/// \brief Open UDP port for receiving camera messages
///
/// Create socket
///
/// Bind to appropriate address
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
            "%s: usage: %s [-h IP] [-p Port] [-t ttl] [-d debug file ] [-D 0|1|2]\n",
            MyName, MyName);
}
