#include "types.h"

// Set palette color
// A - Number of color in the palette B - Color
// Exit: AF|BC|DE|HL corrupted
#define SCR_SET_INK 0xBC32
void firm_set_palette_color(U8 number, U16 color) __sdcccall(1) {
  (void) number;
  (void) color;

__asm
  ld    b,  d
  ld    c,  e
  call  SCR_SET_INK
__endasm;
}
