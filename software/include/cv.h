//	cv.h
//
//  general defines for cyborg_vision stuff
//

#ifndef _CV_H_
#define _CV_H_   1



#define	DEBUG_OFF       0
#define	DEBUG_INFO      1
#define	DEBUG_DETAIL    2

#define TTL_DEFAULT     3   // secs



/// \brief General purpose memory allocator
///
/// Will always return success; exits with a warning message otherwise
void    *cvAlloc(size_t);

/// \brief General purpose memory free
///
void    cvFree(void *);


void	initMutex();




#endif  /* cv.h */
