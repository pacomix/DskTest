#include "types.h"

// Set border and all inks colors. Colors in hardware format
// DE - Address of ink vector - 16 bytes 0-border color 1-ink1 Color 2-ink2 color .....
// Exit: AF corrupt
#define MC_SET_INKS 0xBD25
void firm_set_palette_colors(U8* palette) __sdcccall(1) {
  (void) palette;

__asm
  ex  de, hl
  call MC_SET_INKS
__endasm;
}
