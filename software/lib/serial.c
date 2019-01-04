#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>


/// \brief Open serial port for communicating with the status server
///
/// sets to 8n1 @ specified speed
///
/// returns fd for serial port
int
openSerialPort(char *portAtSpeed)
{
    char            *ptr, copyPortAtSpeed[MAXPATHLEN];
    int             speed, fd;
    speed_t         speedVal;
    struct termios  settings;

    if (strlen(portAtSpeed) > MAXPATHLEN) {
        fprintf(stderr, "%s(): error: serial port specification too long \"%s\"\n",
                __func__, portAtSpeed);
        exit(1);
    }

    strcpy(copyPortAtSpeed, portAtSpeed);

    ptr = copyPortAtSpeed;
    while (*ptr && *ptr != '@') {
        ptr++;
    }

    if ( ! ptr) {
        fprintf(stderr, "%s(): error: invalid 'port@speed' parameter\n",
                __func__);
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
        fprintf(stderr, "%s(): error: invalid serial port speed (%d) for port \"%s\"\n",
                __func__, speed, copyPortAtSpeed);
        exit(1);

    }

    if ((fd = open(copyPortAtSpeed,  O_RDWR)) == -1) {
        fprintf(stderr, "%s(): error: cannot open status server port \"%s\" (%s)\n",
                __func__, copyPortAtSpeed,  strerror(errno));
        exit(1);
    }

    if (tcgetattr(fd, &settings) == -1) {
        fprintf(stderr,
                "%s(): error: cannot get settings for status server port \"%s\" (%s)\n",
                __func__, copyPortAtSpeed,  strerror(errno));
        exit(1);
    }

    cfsetspeed(&settings, speedVal);

    settings.c_cflag &= ~(CSIZE | PARENB);
    settings.c_cflag |= CS8 | CLOCAL;

    if (tcsetattr(fd, TCSANOW, &settings) == -1) {
        fprintf(stderr,
                "%s(): error: cannot set settings for status server port \"%s\" (%s)\n",
                __func__, copyPortAtSpeed,  strerror(errno));
        exit(1);
    }

    return fd;
}
