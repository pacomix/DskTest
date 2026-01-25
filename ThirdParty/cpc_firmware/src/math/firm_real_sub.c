#include "types.h"

//
// Subtract the first real value from the second IN PLACE of the first one.
// HL - pointer to array of the first real value. 5bytes
// DE - pointer to array of the second real value. 5bytes
//
// Exit: HL-integer value rounded up or down depending on the decimal content of
//   the entry real value.
#ifdef CPC_MODEL_464
#  define REAL_SUB 0xBD5E
#else
#  define REAL_SUB 0xBD82
#endif
void firm_real_sub(U8* pFirst, U8* pSecond) __sdcccall(1) {
  (void) pFirst;
  (void) pSecond;

__asm
  call  REAL_SUB
__endasm;
}
