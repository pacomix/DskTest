#include "types.h"

// Set screen mode
// A - Mode 0|1|2
// Exits: AF|BC|DE|HL corrupt
#define SCR_SET_MODE 0xBC0E
void firm_set_screen_mode(U8 mode) __sdcccall(1) {
  (void)mode;

__asm
  call  SCR_SET_MODE
__endasm;
}
