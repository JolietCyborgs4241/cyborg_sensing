//  status/status.h
//

#ifndef _STATUS_STATUS_H_
#define _STATUS_STATUS_H_  1


#define MAX_STATUS_LEN              100

/// status LEDs

#define STAT_LED_AUTO               '0'
#define STAT_LED_DB_UP              '1'
#define STAT_LED_DB_ACTIVE          '2'
#define STAT_LED_ROBO_RIO_UP        '3'
#define STAT_LED_ROBO_RIO_FROM      '4'
#define STAT_LED_ROBO_RIO_TO        '5'
#define STAT_LED_MAIN_SENSOR_ACTIVE '6'
#define STAT_LED_CAM_SENSOR_ACTIVE  '7'
#define STAT_LED_EXTRA_1            '8'
#define STAT_LED_EXTRA_2            '9'


#define COLOR_RED_OFF               'X'
#define COLOR_RED_1                 'a'
#define COLOR_RED_2                 'b'
#define COLOR_RED_3                 'c'
#define COLOR_RED_4                 'd'
#define COLOR_RED_5                 'e'
#define COLOR_RED_6                 'f'
#define COLOR_RED_7                 'g'
#define COLOR_RED_8                 'h'
#define COLOR_RED_9                 'i'
#define COLOR_RED_10                'j'
#define COLOR_RED_11                'k'
#define COLOR_RED_12                'l'
#define COLOR_RED_13                'm'
#define COLOR_RED_14                'n'
#define COLOR_RED_15                'o'
#define COLOR_RED_MAX               'M'

#define COLOR_GREEN_OFF             COLOR_RED_OFF
#define COLOR_GREEN_1               COLOR_RED_1
#define COLOR_GREEN_2               COLOR_RED_2
#define COLOR_GREEN_3               COLOR_RED_3
#define COLOR_GREEN_4               COLOR_RED_4
#define COLOR_GREEN_5               COLOR_RED_5
#define COLOR_GREEN_6               COLOR_RED_6
#define COLOR_GREEN_7               COLOR_RED_7
#define COLOR_GREEN_8               COLOR_RED_8
#define COLOR_GREEN_9               COLOR_RED_9
#define COLOR_GREEN_10              COLOR_RED_10
#define COLOR_GREEN_11              COLOR_RED_11
#define COLOR_GREEN_12              COLOR_RED_12
#define COLOR_GREEN_13              COLOR_RED_13
#define COLOR_GREEN_14              COLOR_RED_14
#define COLOR_GREEN_15              COLOR_RED_15
#define COLOR_GREEN_MAX             COLOR_RED_MAX

#define COLOR_BLUE_OFF              COLOR_RED_OFF
#define COLOR_BLUE_1                COLOR_RED_1
#define COLOR_BLUE_2                COLOR_RED_2
#define COLOR_BLUE_3                COLOR_RED_3
#define COLOR_BLUE_4                COLOR_RED_4
#define COLOR_BLUE_5                COLOR_RED_5
#define COLOR_BLUE_6                COLOR_RED_6
#define COLOR_BLUE_7                COLOR_RED_7
#define COLOR_BLUE_8                COLOR_RED_8
#define COLOR_BLUE_9                COLOR_RED_9
#define COLOR_BLUE_10               COLOR_RED_10
#define COLOR_BLUE_11               COLOR_RED_11
#define COLOR_BLUE_12               COLOR_RED_12
#define COLOR_BLUE_13               COLOR_RED_13
#define COLOR_BLUE_14               COLOR_RED_14
#define COLOR_BLUE_15               COLOR_RED_15
#define COLOR_BLUE_MAX              COLOR_RED_MAX


/// status message to status server consists of 4 bytes
///
/// LED   R value   G value   B value
///
/// LED - one of the above defined STAT_LED_* #define
/// R, G, B value - 'X', and 'a'-'p'
///
/// This makes the protocol all ASCII and provides 16 distinct
/// LED brightness levels.  The API will take 0-255 brightness
/// levels and map them within this range
///
/// 0       -> 'X'
/// 1-15    -> 'a'
/// 16-31   -> 'b'
/// 32-47   -> 'c'
/// 48-63   -> 'd'
/// ...
/// 16-239   -> 'o'
/// 240-255  -> 'M'
///
/// protocol to driver hardware brackets the 4 bytes with '<' and '>'
///
/// this allows us to detect a potetial framing error and recover from it



/// \brief send status update to status server
///
/// LED, R, G, B values
///
/// returns 0 if error - otherwise returns number of bytes sent (should be 4)
int    sendStatusUpdate(char, char, char, char);



/// \brief  open connection to status server
///
/// since actual connection is UDP, it just creates the socket so no
/// actual "connection" is established
void   openStatusConnection(HOST_INFO *host);


typedef struct {

char    led, red, green, blue;

} STATUS_MSG;


#endif  /* status/status.h */
