#ifndef CPC_FIRMWARE_KEYS
#define CPC_FIRMWARE_KEYS

#include "firmware.h"

// NOTE - Firmware functions uses ASCII codes for common keys
// but special ones for others like Fx keys, cursor keys, etc...
// So it is safe to use something like the below to check for keypresses:
// 
// if ('\n' == firm_read_key())
//
#define FIRM_KEY_ENTER_BIG 0x0D

// Amstrad specific
#define FIRM_KEY_ENTER_SMALL 0x8B
#define FIRM_KEY_COPY 0xE0
#define FIRM_KEY_CURSOR_UP 0xF0
#define FIRM_KEY_CURSOR_DOWN 0xF1
#define FIRM_KEY_CURSOR_LEFT 0xF2
#define FIRM_KEY_CURSOR_RIGHT 0xF3

U8 firm_get_key_wait(void) __sdcccall(1);
U8 firm_read_key    (void) __sdcccall(1);

#endif
