#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>




int
main()
{
    struct timeval tv;

    gettimeofday(&tv, (struct timezone *)NULL);

    printf("%ld.%ld\n", tv.tv_sec, tv.tv_usec);

    exit(0);
}
