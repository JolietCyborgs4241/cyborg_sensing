//  cyborg_lib.h
//
//  general defines for cyborg library
//

#ifndef _CYBORG_LIB_H_
#define _CYBORG_LIB_H_   1



void    *cvAlloc(size_t);
void    cvFree(void *);


/// parameter is "port@speed"
///
/// returns fd for open serial port set to 8N1 @ specified speed
/// (speed is validated between 9600 and 230400 baud)
int     openSerialPort(char *);

#endif  /* cyborg_lib.h */

