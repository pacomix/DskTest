#include "txt.h"

#define TXT_SET_ROW 0xBB72
void firm_set_cursor_at_y(U8 row) __sdcccall(1) {
  (void) row;

__asm
  call  TXT_SET_ROW
__endasm;
}
