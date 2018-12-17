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
#include "gen_sss/sensors.h"
#include "gen_sss/externs.h"



void    init(int, char **), process9dofRecord(char *),
        setAndSend(char *, char *, int, int, int);



int
main(int argc, char **argv)

{
	MyName = argv[0];

	DebugFP = stderr;
    
    init(argc, argv);

    KickOffSensorThreads(Sensors);

	while (1) {
        pause();                // don't do anything but don't burn CPU
    }
}
