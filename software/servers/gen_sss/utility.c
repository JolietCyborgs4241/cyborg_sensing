//  utility.c
//
//  utility functions for gen_sss

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "cv.h"
#include "cv_net.h"
#include "sensors.h"
#include "db/externs.h"


/// \brief  cyborg vision malloc front end
///
/// exits with an error to DebugFP if malloc() fails
void *
cvAlloc(size_t bytes)

{
    void *ptr;

    if ( ! (ptr = malloc(bytes)) ) {
        fprintf(DebugFP, "%s: alloc error (%d bytes)\n", MyName, (int)bytes);
        exit(1);
    }

    return ptr;
}



/// \brief  cyborg vision free front end
///
/// doesn't do anything special right now - calls free() directly
void
cvFree(void *ptr)
{
    free(ptr);
}
