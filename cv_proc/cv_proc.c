//	cv_proc.c
//
//	main driver for vision communications

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
#include "cv_cam.h"
#include "cv_lists.h"
#include "cv_threads.h"
#include "cv_externs.h"




void    init(int, char **);





int
main(int argc, char **argv)

{
	MyName = argv[0];

    DebugFP = stderr;           // unless overridden on the command line

	init(argc, argv);

    startCamDataThread(HostInfo.sock);   // start reading recs from the cams

    startPruneThread(Ttl);      // get rid of records older than TTL

	while (1) {

		sleep(1000);            // eventually the processing from the RoboRio
                                // module will happen here in the main thread
                                //
                                // for now, we'll just sleep & sleep & sleep
	}

}
