#include "txt.h"

// Outputs a char to the screen (where?????)
// A - Char to output
#define TXT_OUTPUT 0xBB5A
void firm_put_char(U8 character) __sdcccall(1) {
  (void) character;
  
__asm
  call TXT_OUTPUT
__endasm;
}
