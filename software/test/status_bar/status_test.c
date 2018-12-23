//	status_test.c
//
//  runs a series of random LEDs and colors and then a sequence of color
//  across the entire strip

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

#define OF     COLOR_OFF
#define PU     COLOR_PURPLE
#define BL     COLOR_BLUE
#define GR     COLOR_GREEN
#define RE     COLOR_RED
#define OR     COLOR_ORANGE
#define YE     COLOR_YELLOW
#define WH     COLOR_WHITE

#define SEQ_COUNT   30
#define SEQ_LEN     15

unsigned char    sequence[SEQ_COUNT][SEQ_LEN] = {
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ PU, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ BL, PU, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ GR, BL, PU, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ RE, GR, BL, PU, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ OR, RE, GR, BL, PU, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ YE, OR, RE, GR, BL, PU, OF, OF, OF, OF, OF, OF, OF, OF, OF },
{ WH, YE, OR, RE, GR, BL, PU, OF, OF, OF, OF, OF, OF, OF, OF },
{ YE, WH, YE, OR, RE, GR, BL, PU, OF, OF, OF, OF, OF, OF, OF },
{ OR, YE, WH, YE, OR, RE, GR, BL, PU, OF, OF, OF, OF, OF, OF },
{ RE, OR, YE, WH, YE, OR, RE, GR, BL, PU, OF, OF, OF, OF, OF },
{ GR, RE, OR, YE, WH, YE, OR, RE, GR, BL, PU, OF, OF, OF, OF },
{ BL, GR, RE, OR, YE, WH, YE, OR, RE, GR, BL, PU, OF, OF, OF },
{ PU, BL, GR, RE, OR, YE, WH, YE, OR, RE, GR, BL, PU, OF, OF },
{ OF, PU, BL, GR, RE, OR, YE, WH, YE, OR, RE, GR, BL, PU, OF },
{ OF, OF, PU, BL, GR, RE, OR, YE, WH, YE, OR, RE, GR, BL, PU },
{ OF, OF, OF, PU, BL, GR, RE, OR, YE, WH, YE, OR, RE, GR, BL },
{ OF, OF, OF, OF, PU, BL, GR, RE, OR, YE, WH, YE, OR, RE, GR },
{ OF, OF, OF, OF, OF, PU, BL, GR, RE, OR, YE, WH, YE, OR, RE },
{ OF, OF, OF, OF, OF, OF, PU, BL, GR, RE, OR, YE, WH, YE, OR },
{ OF, OF, OF, OF, OF, OF, OF, PU, BL, GR, RE, OR, YE, WH, YE },
{ OF, OF, OF, OF, OF, OF, OF, OF, PU, BL, GR, RE, OR, YE, WH },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, PU, BL, GR, RE, OR, YE },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, PU, BL, GR, RE, OR },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, PU, BL, GR, RE },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, PU, BL, GR },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, PU, BL },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, PU },
{ OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF, OF } };



void    init(int, char **);

extern  int MaxDelay, RandomCount;


int
main(int argc, char **argv)

{
    int randLed, randColor, randDelay, randTimeoutLed;
    int retVal, i, ii, loops;
    unsigned char    cmdToStatusBar;

    MyName = argv[0];

	DebugFP = stderr;
    
    init(argc, argv);

    gettimeofday(&StartTime, NULL);

    cmdToStatusBar = SET_LED_TIMEOUT | (char)LedTtl;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s: sending timeout setting to status bar (0x%x)\n",
                MyName, cmdToStatusBar);
    }

    retVal = write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

    if (retVal == -1) {
        fprintf(stderr, "%s: error: write error to \"%s\" - returned %d (%s)\n",
                MyName, SerialPort, retVal, strerror(errno));
        exit(1);
    }

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s: Starting random sequence for %d writes\n",
                MyName, RandomCount);
    }

	while (1) {
        for (i = 0 ; i < RandomCount ; i++) {
            randLed  = random() % (sizeof(leds) / sizeof(leds[0]));
            randColor = random() % (sizeof(colors) / sizeof(colors[0]));
            randDelay = random() % MaxDelay;    // msecs

            cmdToStatusBar = leds[randLed] | colors[randColor];

            if (DebugLevel >= DEBUG_DETAIL) {
                fprintf(DebugFP, "%s: randLed = %d, randColor = %d, randDelay = %d\n",
                        MyName, randLed, randColor, randDelay);
                fprintf(DebugFP, "%s: cmdToStatusBar 0x%02x\n",
                        MyName, cmdToStatusBar);
            }

            retVal = write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

            if (retVal == -1) {
                fprintf(stderr, "%s: error: write error to \"%s\" - returned %d (%s)\n",
                        MyName, SerialPort, retVal, strerror(errno));
                exit(1);
            }

            usleep(randDelay * 1000);       // convert msecs into usecs
        }

        if (DebugLevel >= DEBUG_DETAIL) {
            fprintf(DebugFP, "%s: Starting color sequence\n",
                    MyName);
        }

        for (loops = 0 ; loops < 4 ; loops++) {
            for (i = 0 ; i < SEQ_COUNT ; i++) {
                for (ii = 0 ; ii < SEQ_LEN ; ii++) {

                    cmdToStatusBar = (ii << 4) | sequence[i][ii];

                    if (DebugLevel >= DEBUG_DETAIL) {
                        fprintf(DebugFP,
                                "%s: sequence [%d][%d] = 0x%02x - cmd = 0x%02x\n",
                                MyName, i, ii, sequence[i][ii], cmdToStatusBar);
                    }

                    retVal = write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

                    if (retVal == -1) {
                        fprintf(stderr,
                                "%s: error: write error to \"%s\" - returned %d (%s)\n",
                                MyName, SerialPort, retVal, strerror(errno));
                        exit(1);
                    }

                }
                usleep(50000);
            }
        }

        for (i = 0 ; i < 15 ; i++) {                // turn all off
            cmdToStatusBar = (i << 4);
            write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));
        }

        // strobe one color down the whole strip
        //
        //  repeat for all colors
        for (i = 1 ; i < 8 ; i++) {             // colors (not off)
            for (ii = 0 ; ii < 15 ; ii++) {     // LEDs

                if (DebugLevel >= DEBUG_DETAIL) {
                    fprintf(DebugFP,
                            "%s: color loop %d\n",
                            MyName, i);
                }

                cmdToStatusBar = (ii << 4) | i;
                write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

                usleep(40000);

                cmdToStatusBar = (ii << 4);
                write(SerialFd, &cmdToStatusBar, sizeof(cmdToStatusBar));

                usleep(10000);
            }
        }
    }
}


