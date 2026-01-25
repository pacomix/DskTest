#include "txt.h"

#define TXT_SET_COLUMN 0xBB6F
void firm_set_cursor_at_x(U8 column) __sdcccall(1) {
  (void) column;

__asm
  call  TXT_SET_COLUMN
__endasm;
}
