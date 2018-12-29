//	cv_db.c
//
//	database for autonomous mode operations communications

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "cv.h"
#include "cv_net.h"
#include "db/lists.h"
#include "db/threads.h"
#include "db/externs.h"
#include "status/status.h"



void    init(int, char **);





int
main(int argc, char **argv)

{
	MyName = argv[0];

    DebugFP = stderr;               // unless overridden on the command line

	init(argc, argv);

    startSensorDataThread(HostInfo.sock);   // start reading from sensors

    startPruneThread();             // get rid of records older than TTL

	while (1) {

        sendStatusUpdate(STAT_LED_DB_UP, COLOR_GREEN);
        
		sleep(1);                   // heartbeat to the status display to
                                    // indicate the database is still up
                                    // and running

                                    // heartbeat to the status display to
                                    // indicate the general sensor server
                                    // is still up and running
	}

}
