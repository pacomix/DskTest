#include "txt.h"

// Outputs a char to the specfied location in screen
// A - Char to output H-Column L-Line
// AF|BC|DE|HL corrupt
#define TXT_WRITE_CHAR 0xBDD3
void firm_put_char_at(U8 character, U8 line, U8 column) __sdcccall(1) {
  (void) character;
  (void) line;
  (void) column;

__asm
  push  ix
  ld    ix, #0
  add   ix, sp

  ld    h,  4 (ix)
  call  TXT_WRITE_CHAR

  pop ix
__endasm;
}
