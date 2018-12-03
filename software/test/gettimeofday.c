#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>




int
main()
{
    struct timeval tv;

    gettimeofday(&tv, (struct timezone *)NULL);

#ifdef	__APPLE__
    printf("%ld.%d\n", tv.tv_sec, tv.tv_usec);
#else	// everything else
    printf("%ld.%ld\n", tv.tv_sec, tv.tv_usec);
#endif

    exit(0);
}
