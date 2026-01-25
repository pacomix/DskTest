#include "types.h"

// Set border color
// B - border color
// Exit: AF|BC|DE|HL corrupted
#define SCR_SET_BORDER 0xBC38
void firm_set_border_color(U8 color1, U8 color2) __sdcccall(1) {
  (void) color1;
  (void) color2;

__asm
  ld    b,  a
  ld    c,  l
  call  SCR_SET_BORDER
__endasm;
}
