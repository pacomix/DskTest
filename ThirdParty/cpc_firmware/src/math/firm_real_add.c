#include "types.h"

//
// Add the first real value to the second IN PLACE of the first one.
// HL - pointer to array of the first real value. 5bytes
// DE - pointer to array of the second real value. 5bytes
//
// Exit: HL-integer value rounded up or down depending on the decimal content of
//   the entry real value.
#ifdef CPC_MODEL_464
#  define REAL_ADD 0xBD58
#else
#  define REAL_ADD 0xBD7C
#endif
void firm_real_add(U8* pFirst, U8* pSecond) __sdcccall(1) {
  (void) pFirst;
  (void) pSecond;

__asm
  call  REAL_ADD
__endasm;
}
