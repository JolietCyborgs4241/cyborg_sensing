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
/// 240-255   -> 'p'

/// \brief send status update to status server
///
/// LED, R, G, B values
void    sendStatusUpdate(char, int, int, int);



typedef struct {

char    led, red, green, blue;

} STATUS_MSG;


#endif  /* status/status.h */
