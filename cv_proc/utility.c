//  utility.c
//
//  utility functions for cv_proc

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "cv.h"
#include "cv_net.h"
#include "cv_externs.h"


/// \brief  cyborg vision malloc front end
///
/// exits with an error to stderr if malloc() fails
void *
cvAlloc(size_t bytes)

{
    void *ptr;

    if ( ! (ptr = malloc(bytes)) ) {
        fprintf(DebugFP, "%s: alloc error (%d bytes)\n", MyName, (int)bytes);
        exit(1);
    }

#ifdef  DEBUG
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(%ld) returning (0x%lx)\n", __func__, bytes, (long)ptr);
    }
#endif  // DEBUG

    return ptr;
}



/// \brief  cyborg vision free front end
///
/// doesn't do anything special rifht now - calls free() directly
void
cvFree(void *ptr)
{
#ifdef  DEBUG
    if (DebugLevel == DEBUG_DETAIL) {
        fprintf(DebugFP, "%s(0x%lx)\n", __func__, (long)ptr);
    }
#endif  // DEBUG

    free(ptr);
}
