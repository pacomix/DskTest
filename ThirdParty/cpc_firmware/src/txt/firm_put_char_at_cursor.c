#include "txt.h"

// Outputs a char to the current cursor location in screen
// A - Char to output
// AF|BC|DE|HL corrupt
#define TXT_WR_CHAR 0xBB5D
void firm_put_char_at_cursor(U8 character) __sdcccall(1) {
  (void) character;

__asm
  call TXT_WR_CHAR
__endasm;
}
