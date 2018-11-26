//  utility.c
//
//  utility functions for cv_proc

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_externs.h"


void *
cvAlloc(size_t bytes)

{
    void *ptr;

    if ( ! (ptr = malloc(bytes)) ) {
        fprintf(stderr, "%s: alloc error (%d bytes)\n", MyName, (int)bytes);
        exit(1);
    }

    return ptr;
}




void
cvFree(void *ptr)
{
    free(ptr);
}
