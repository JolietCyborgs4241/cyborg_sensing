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




static  void        usage(), openOutgoingPort(HOST_INFO *),
                    dumpConfig();

static  int         openSerialPort(char *, int);


/// \brief process command line and perform other initializations
///
/// -h Host IP of database
///
/// -p Host port of database
///
/// -c serial port to read from
///
/// -s serial port speed
///
/// -d Debug output file
///
/// -D Debug level
///
/// Also opens UDP socket for sending messages to database
void
init(int argc, char **argv)

{
    int c, ttl;

    // clear HostInfo structure
    HostInfo.hostIPString = DEF_HOST_IP_STRING;
    HostInfo.hostPort     = DEF_HOST_PORT_DB_POST;  // port to send to add data

    while ((c = getopt(argc, argv, "h:p:c:s:D:d:t:l:I:S:")) != -1) {

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

        case 'I':
            SensorId = optarg;
            break;

        case 'S':
            SensorSubId = optarg;
            break;

        case 'c':
            SerialPort = optarg;
            break;

        case 's':
            SerialSpeed = atoi(optarg);
            if (SerialSpeed < 0) {  // other checks happen later
                fprintf(stderr, "%s: error: serial port speed value must be >= 0\n",
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
        fprintf(stderr, "%s: error: serial port (-c) must be specified\n",
                MyName);
        exit(1);
    }

    if (SerialSpeed == 0) {
        fprintf(stderr, "%s: error: serial speed (-s) must be specified\n",
                MyName);
        exit(1);
    }

    if (strlen(SensorId) == 0) {
        fprintf(stderr, "%s: error: SensorID must be specified\n",
                MyName);
        exit(1);
    }
    openOutgoingPort(&HostInfo);

    SerialFd = openSerialPort(SerialPort, SerialSpeed);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    fprintf(DebugFP, "DumpConfig():\n");
    fprintf(DebugFP, "Network:\n\tSending to:\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);

    fprintf(DebugFP, "Serial:\t\"%s\" @ %d baud\n", SerialPort, SerialSpeed);

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



/// \brief Open UDP port for receiving 9dof sensor messages
///
/// sets to 8n1 @ specified speed
//
/// returns fd
static int
openSerialPort(char *port, int speed)
{
    int             fd;
    speed_t         speedVal;
    struct termios  settings;

    if ((fd = open(port, O_RDWR)) == -1) {
        fprintf(stderr, "%s: error: cannot open 9dof port \"%s\" (%s)\n",
                MyName, port, strerror(errno));
        exit(1);
    }

    if (tcgetattr(fd, &settings) == -1) {
        fprintf(stderr,
                "%s: error: cannot get settings for 9dof port \"%s\"(%s)\n",
                MyName, port, strerror(errno));
        exit(1);
    }

    switch (speed) {

    case 9600:
        speedVal = B9600;
        break;

    case 19200:
        speedVal = B19200;
        break;

    case 38400:
        speedVal = B38400;
        break;

    case 57600:
        speedVal = B57600;
        break;

    case 115200:
        speedVal = B115200;
        break;

    case 23400:
        speedVal = B230400;
        break;

    default:
        fprintf(stderr, "%s: error: invalid serial port speed (%d)\n",
                MyName, speed);
        exit(1);
    }

    cfsetspeed(&settings, speedVal);

    settings.c_cflag &= ~(CSIZE | PARENB);
    settings.c_cflag  = CS8 | CLOCAL;

    if (tcsetattr(fd, TCSANOW, &settings) == -1) {
        fprintf(stderr,
                "%s: error: cannot set settings for 9dof port \"%s\"(%s)\n",
                MyName, port, strerror(errno));
        exit(1);
    }

    return fd;
}



/// \brief Open UDP port for sending messages to database
///
/// Create socket
///
/// Socket is set in HOST_INFO structure
static void
openOutgoingPort(HOST_INFO *host)
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
}



/// \brief Usage message
///
/// Shown is any invalid parameters specvified or '-?' on command line
static void
usage()

{
    fprintf(stderr,
            "%s: usage: %s [-h IP] [-p Port] [-I ID] [-S SubID] [-c 9dof comm port] [-s speed] [-d debug file ] [-D 0|1|2|3]\n",
            MyName, MyName);
}
