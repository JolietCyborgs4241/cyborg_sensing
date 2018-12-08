//	cv.h
//
//  general defines for cyborg_vision stuff
//

#ifndef _CV_H_
#define _CV_H_   1



#define	DEBUG_OFF           0
#define	DEBUG_INFO          1
#define	DEBUG_DETAIL        2

#define DEBUG_OUT(X, ...)   fprintf(DebugFP X__VA_OPT__(,) __VA_ARGS__)
//#define G(X, ...) f(0, X __VA_OPT__(,) __VA_ARGS__)
//G(a, b, c) // replaced by f(0, a, b, c)
//G(a, )     // replaced by f(0, a)
//G(a)       // replaced by f(0, a)




#define PRUNE_FREQUENCY         500 // msecs
#define TTL_SECS_DEFAULT        3   // secs
#define TTL_USECS_DEFAULT       0   // microseconds



/// \brief General purpose memory allocator
///
/// Will always return success; exits with a warning message otherwise
void    *cvAlloc(size_t);

/// \brief General purpose memory free
///
void    cvFree(void *);


#endif  /* cv.h */
