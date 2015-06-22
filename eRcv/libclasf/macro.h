#ifndef __MACRO_H__
#define __MACRO_H__

//EVENT TYPES
#define EV_SYN		0x00
#define EV_KEY		0x01
#define EV_ABS		0x03

//Synchronization events
#define SYN_REPORT	0

//Absolute
#define ABS_MT_TRACKING_ID      0x39    /* Unique ID of initiated contact */
#define ABS_MT_POSITION_X       0x35    /* Center X touch position */
#define ABS_MT_POSITION_Y       0x36    /* Center Y touch position */

#define MAX_TAP_DISTANCE		50

#define DLLEXPORT				__declspec(dllexport)

#endif