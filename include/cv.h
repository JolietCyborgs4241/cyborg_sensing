//	cv.h
//
//  general defines for cyborg_vision stuff
//

#ifndef _CV_H_
#define _CV_H_   1


extern	char        *MyName;



#define	DEBUG_OFF       0
#define	DEBUG_INFO      1
#define	DEBUG_DETAIL    2

extern	int         DebugLevel;



void	cvProcInit(int, char **);



#endif  /* cv.h */
