#include "txt.h"

#define TXT_INVERSE 0xBB9C
void firm_set_inverse(void) __sdcccall(1) {
__asm
  call  TXT_INVERSE
__endasm;
}
