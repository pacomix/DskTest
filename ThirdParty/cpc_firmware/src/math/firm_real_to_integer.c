#include "types.h"

// Converts a real value into integer (16bits) (float)
// HL - pointer to array of real value. 5bytes
// Exit: HL-integer value rounded up or down depending on the decimal content of
//   the entry real value.
#ifdef CPC_MODEL_464
#  define REAL_TO_INTEGER 0xBD46
#else
#  define REAL_TO_INTEGER 0xBD6A
#endif
U16 firm_real_to_integer(U8* pReal) __sdcccall(1) {
  (void) pReal;

__asm
  call  REAL_TO_INTEGER
  ex    de,hl
__endasm;
}
