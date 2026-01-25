#include "txt.h"

// Set cursor location
// H - column L - line
// Exits: AF|HL corrupt
#define TXT_SET_CURSOR 0xBB75
void firm_set_cursor_at(U8 column, U8 line) __sdcccall(1) {
  (void) column;
  (void) line;

__asm
  ld    h,  a
  call  TXT_SET_CURSOR
__endasm;
}
