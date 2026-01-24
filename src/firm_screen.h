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

// Wait for flyback (once the beam has finished drawing the screen to monitor)
#define MC_WAIT_FLYBACK 0xBD19
void firm_wait_flyback(void) __sdcccall(1) {
__asm
  call MC_WAIT_FLYBACK
__endasm;
}


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
