//	gen_sss.c
//
//	generic serial sensor server to read and post sensor
//	readings to sensor database
//
//  supports sensors that provide db-ready output readings
//
//  creates one thread per serial connection

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "cv.h"
#include "cv_net.h"
#include "status/externs.h"
#include "status/status.h"


char    colors[] = { COLOR_OFF, COLOR_RED, COLOR_GREEN, COLOR_BLUE,
                     COLOR_YELLOW, COLOR_PURPLE, COLOR_ORANGE,
                     COLOR_WHITE };

char    leds[]   = { STAT_LED_AUTO, STAT_LED_DB_UP, STAT_LED_DB_ACTIVE,
                     STAT_LED_ROBO_RIO_UP, STAT_LED_ROBO_RIO_FROM,
                     STAT_LED_ROBO_RIO_TO, STAT_LED_MAIN_SENSOR_ACTIVE,
                     STAT_LED_CAM_SENSOR_ACTIVE };


void    init(int, char **);

extern  int MaxDelay;


int
main(int argc, char **argv)

{
    int randLed, randColor, randDelay, randTimeoutLed;
    int retVal;
    char    cmdToStatusBar;

    MyName = argv[0];

	DebugFP = stderr;
    
    init(argc, argv);

    gettimeofday(&StartTime, NULL);

    cmdToStatusBar = SET_LED_TIMEOUT | (char)LedTtl;

    retVal = write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

    if (retVal == -1) {
        fprintf(stderr, "%s: error: write error to \"%s\" - returned %d (%s)\n",
                MyName, SerialPort, retVal, strerror(errno));
        exit(1);
    }


	while (1) {
        randLed  = random() % (sizeof(leds) / sizeof(leds[0]));
        randColor = random() % (sizeof(colors) / sizeof(colors[0]));
        randDelay = random() % MaxDelay;    // msecs

        cmdToStatusBar = leds[randLed] | colors[randColor];

        retVal = write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

        if (retVal == -1) {
            fprintf(stderr, "%s: error: write error to \"%s\" - returned %d (%s)\n",
                    MyName, SerialPort, retVal, strerror(errno));
            exit(1);
        }

        usleep(randDelay );
    }
}





