//  status/status.h
//

#ifndef _STATUS_STATUS_H_
#define _STATUS_STATUS_H_  1


#define MAX_STATUS_LEN              100

/// status LEDs
///
/// first 4 bits of LED command
///
/// the API to the status server treats these as a separate parameter
#define STAT_LED_AUTO               0x00
#define STAT_LED_DB_UP              0x10
#define STAT_LED_DB_ACTIVE          0x20
#define STAT_LED_ROBO_RIO_UP        0x30
#define STAT_LED_ROBO_RIO_FROM      0x40
#define STAT_LED_ROBO_RIO_TO        0x50
#define STAT_LED_MAIN_SENSOR_ACTIVE 0x60
#define STAT_LED_CAM_SENSOR_ACTIVE  0x70
#define STAT_LED_EXTRA_1            0x80
#define STAT_LED_EXTRA_2            0x90
#define STAT_LED_EXTRA_3            0xa0
#define STAT_LED_EXTRA_4            0xb0
#define STAT_LED_EXTRA_5            0xc0
#define STAT_LED_EXTRA_6            0xd0
#define STAT_LED_EXTRA_7            0xe0

#define SET_LED_TIMEOUT             0xf0


/// color palette
///
/// last 4 bits of LED command
///
/// the API to the status server treats these as a separate parameter

#define COLOR_OFF                   0x00
#define COLOR_RED                   0x01
#define COLOR_GREEN                 0x02
#define COLOR_BLUE                  0x03
#define COLOR_YELLOW                0x04
#define COLOR_PURPLE                0x05
#define COLOR_ORANGE                0x06
#define COLOR_WHITE                 0x07


/// status message to status server consists of r bytes
///
/// LED   color
///
/// LED - one of the above defined STAT_LED_* #define
/// color - one of the above defined colors



/// \brief send status update to status server
///
/// LED, color
///
/// returns 0 if error - otherwise returns number of bytes sent
int    sendStatusUpdate(unsigned char, unsigned char);

#define DEFAULT_LED_TTL         3   // secs
#define MAX_LED_TTL             15  // secs


/// \brief set LED TTL on the status bar (applies to all LEDs)
///
/// time (secs)
///
/// must be > 0 and <= MAX_LED_TTL
///
/// returns 0 if error
int    sendLedTtl(char);



/// \brief  open connection to status server
///
/// since actual connection is UDP, it just creates the socket so no
/// actual "connection" is established
void   openStatusConnection(HOST_INFO *host);


typedef struct {

char    led, color;

} STATUS_MSG;


#endif  /* status/status.h */
