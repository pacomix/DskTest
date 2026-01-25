#include "types.h"

// Set all inks and border to the same color. colors in hardware format
// DE - Address of ink vector - 2 bytes 0-border color 1-inks Color
// Exit: AF corrupt
#define MC_CLEAR_INKS 0xBD22
void firm_clear_palette_colors(U8 border, U8 ink) __sdcccall(1) {
  (void) border;
  (void) ink;

__asm
  ld    h,  a
  ld    (#__clear_colors), hl
  ;ld    (#__clear_colors+1), l
  ld    de, #__clear_colors
  call MC_CLEAR_INKS

__clear_colors: .db #0x00, #0x00
__endasm;
}
