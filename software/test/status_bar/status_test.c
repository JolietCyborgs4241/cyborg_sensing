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


unsigned char    colors[] = { COLOR_OFF, COLOR_RED, COLOR_GREEN, COLOR_BLUE,
                              COLOR_YELLOW, COLOR_PURPLE, COLOR_ORANGE,
                              COLOR_WHITE };

unsigned char    leds[]   = { STAT_LED_AUTO, STAT_LED_DB_UP, STAT_LED_DB_ACTIVE,
                              STAT_LED_ROBO_RIO_UP, STAT_LED_ROBO_RIO_FROM,
                              STAT_LED_ROBO_RIO_TO, STAT_LED_MAIN_SENSOR_ACTIVE,
                              STAT_LED_CAM_SENSOR_ACTIVE,
                              STAT_LED_EXTRA_1, STAT_LED_EXTRA_2 };

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



void    init(int, char **), sendIt(unsigned char, unsigned char);

extern  int MaxDelay, RandomCount;


int
main(int argc, char **argv)

{
    int randLed, randColor, randDelay, randTimeoutLed;
    int retVal, i, ii, loops;

    MyName = argv[0];

	DebugFP = stderr;
    
    init(argc, argv);

    gettimeofday(&StartTime, NULL);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s: sending timeout setting to status bar\n",
                MyName);
    }

    sendIt(SET_LED_TIMEOUT, (unsigned char)LedTtl);

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s: Starting random sequence for %d writes\n",
                MyName, RandomCount);
    }

	while (1) {
        for (i = 0 ; i < RandomCount ; i++) {
            randLed  = random() % (sizeof(leds) / sizeof(leds[0]));
            randColor = random() % (sizeof(colors) / sizeof(colors[0]));
            randDelay = random() % MaxDelay;    // msecs

            if (DebugLevel >= DEBUG_DETAIL) {
                fprintf(DebugFP, "%s: randLed = %d, randColor = %d, randDelay = %d\n",
                        MyName, randLed, randColor, randDelay);
            }

            sendIt(leds[randLed], colors[randColor]);

            usleep(randDelay * 1000);       // convert msecs into usecs
        }

        if (DebugLevel >= DEBUG_DETAIL) {
            fprintf(DebugFP, "%s: Starting color sequence\n",
                    MyName);
        }

        for (loops = 0 ; loops < 4 ; loops++) {
            for (i = 0 ; i < SEQ_COUNT ; i++) {
                for (ii = 0 ; ii < SEQ_LEN ; ii++) {

                    if (DebugLevel >= DEBUG_DETAIL) {
                        fprintf(DebugFP,
                                "%s: sequence [%d][%d] = 0x%02x\n",
                                MyName, i, ii, sequence[i][ii]);
                    }

                    sendIt(ii << 4, sequence[i][ii]);
                }
                usleep(50000);
            }
        }

        for (i = 0 ; i < 15 ; i++) {                // turn all off
            sendIt(i, 0);
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

                sendIt(ii << 4, i);

                usleep(40000);

                sendIt(ii << 4, 0);;

                usleep(10000);
            }
        }
    }
}




void
sendIt(unsigned char led, unsigned char color)
{
    unsigned char   serialCmd;

    serialCmd = led | color;

    if (DebugLevel >= DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%02x, 0x%02x): serialCmd = 0x%02x\n",
                __func__, led, color, serialCmd);
    }

    if (SerialPort) {
        write(SerialFd, &serialCmd, 1);
    } else {
        sendStatusUpdate(led, color);
    }
}
