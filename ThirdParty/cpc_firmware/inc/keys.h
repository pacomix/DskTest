#ifndef CPC_FIRMWARE_KEYS
#define CPC_FIRMWARE_KEYS

#include "firmware.h"

#define CHAR_ENTER_BIG 0x0D
#define CHAR_ENTER_SMALL 0x8B
#define CHAR_COPY 0xE0

#define CHAR_CURSOR_UP 0xF0
#define CHAR_CURSOR_DOWN 0xF1
#define CHAR_CURSOR_LEFT 0xF2
#define CHAR_CURSOR_RIGHT 0xF3

U8 firm_get_key_wait(void) __sdcccall(1);
U8 firm_read_key    (void) __sdcccall(1);

#endif
