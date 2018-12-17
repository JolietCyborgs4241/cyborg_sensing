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
#include "gen_sss/sensors.h"
#include "gen_sss/externs.h"




static  void        usage(), openOutgoingPort(HOST_INFO *),
                    openSerialPort(SENSOR_CONN *),
                    openSensorConns(), dumpConfig();


/// \brief process command line and perform other initializations
///
/// -h Host IP of database
///
/// -p Host port of database
///
/// -s serial port @ speed to read from
///
/// -d Debug output file
///
/// -D Debug level
///
/// -L Log output file
///
/// Also opens UDP socket for sending messages to database
void
init(int argc, char **argv)

{
    int c, ttl;
    SENSOR_CONN *sensorPtr;

    // clear HostInfo structure
    HostInfo.hostIPString = DEF_HOST_IP_STRING;
    HostInfo.hostPort     = DEF_HOST_PORT_DB_POST;  // port to send to add data

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
            sensorPtr = Sensors;

            if ( ! Sensors) {   // first one
                Sensors = cvAlloc(sizeof(SENSOR_CONN));
                sensorPtr = Sensors;
            } else {            // else find the end of the list
                while (sensorPtr->next) {
                    sensorPtr = sensorPtr->next;
                }
                sensorPtr->next = cvAlloc(sizeof(SENSOR_CONN));
                sensorPtr       = sensorPtr->next;
            }

            sensorPtr->portAtSpeed = cvAlloc(strlen(optarg) + 1);
            strcpy(sensorPtr->portAtSpeed, optarg);
            sensorPtr->next = NULL;
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

    if ( ! Sensors) {
        fprintf(stderr, "%s: error: no sensor connections specified\n",
                MyName);
        exit(1);
    }

    openOutgoingPort(&HostInfo);

    openSensorConns(Sensors);

    if (DebugLevel) {   // any debug level
        dumpConfig();
    }
}



/// Dump command line settings
static void
dumpConfig()
{
    SENSOR_CONN *ptr = Sensors;

    fprintf(DebugFP, "DumpConfig():\n");
    fprintf(DebugFP, "Network:\n\tSending to:\t%s:%d\n\tSock fd:\t%d\n",
           HostInfo.hostIPString, HostInfo.hostPort,
           HostInfo.sock);

    fprintf(DebugFP, "Sensors:\n");
    dumpSensors(Sensors);

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




/// \brief Open all of the serial ports for each defined sensor
///
static void
openSensorConns(SENSOR_CONN *ptr)
{
    while (ptr) {
        openSerialPort(ptr);
        ptr = ptr->next;
    }
}




/// \brief Open serial port for receiving generic sensor messages
///
/// sets to 8n1 @ specified speed
///
/// sets fd in passed sensor structure
///
/// does modify portAtSpeed string in SENSOR_CONN structure
static void
openSerialPort(SENSOR_CONN *sensor)
{
    char            *ptr;
    int             speed, fd;
    speed_t         speedVal;
    struct termios  settings;

    sensor->serialPort = sensor->portAtSpeed;

    ptr = sensor->portAtSpeed;
    while (*ptr && *ptr != '@') {
        ptr++;
    }

    if ( ! ptr) {
        fprintf(stderr, "%s: error: invalid 'port@speed' parameter\n",
                MyName);
        exit(1);
    }

    *ptr = '\0';     // terminate at the @
    ptr++;          // point to speed

    speed = atoi(ptr);

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

    case 230400:
        speedVal = B230400;
        break;

    default:
        fprintf(stderr, "%s: error: invalid serial port speed (%d) for port \"%s\"\n",
                MyName, speed, sensor->serialPort);
        exit(1);

    }

    if ((fd = open(sensor->serialPort, O_RDWR)) == -1) {
        fprintf(stderr, "%s: error: cannot open gen_sss port \"%s\" (%s)\n",
                MyName, sensor->serialPort, strerror(errno));
        exit(1);
    }

    if (tcgetattr(fd, &settings) == -1) {
        fprintf(stderr,
                "%s: error: cannot get settings for gen_sss port \"%s\" (%s)\n",
                MyName, sensor->serialPort, strerror(errno));
        exit(1);
    }

    cfsetspeed(&settings, speedVal);

    settings.c_cflag &= ~(CSIZE | PARENB);
    settings.c_cflag  = CS8 | CLOCAL;

    if (tcsetattr(fd, TCSANOW, &settings) == -1) {
        fprintf(stderr,
                "%s: error: cannot set settings for gen_sss port \"%s\" (%s)\n",
                MyName, sensor->serialPort, strerror(errno));
        exit(1);
    }

    sensor->fd = fd;
}



/// \brief Open UDP port for sending messages to database
///
/// Create socket
///
/// Socket is set in HOST_INFO structure
///
/// every sensor thread uses this socket
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
            "%s: usage: %s [-h IP] [-p Port] [-s serial_port@speed ] [-d debug file ] [-D 0|1|2|3] [-l log file]\n",
            MyName, MyName);
}



void
dumpSensors(SENSOR_CONN *ptr)
{
    while (ptr) {
        dumpSensor(ptr);
        ptr = ptr->next;
    }
}


void
dumpSensor(SENSOR_CONN *ptr)
{
    fprintf(stderr, "Sensor ptr (0x%lx):\n", (long)ptr);
    fprintf(stderr, "\tserialPort:\t\"%s\"\n", ptr->serialPort ? ptr->serialPort : "NULL");
    fprintf(stderr, "\tserialSpeed:\t%d\n", ptr->serialSpeed);
    fprintf(stderr, "\tfd:\t\t%d\n", ptr->fd);
    fprintf(stderr, "\tnext:\t\t0x%lx\n", (long)ptr->next);
}
