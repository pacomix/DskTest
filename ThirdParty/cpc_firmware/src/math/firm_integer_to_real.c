#include "types.h"

// Converts an integer value into real (float)
// HL - Integer value.
// DE - Target address where the result will be stored (5 bytes)
// A - b7-1=Integer negative 0=Integer positive
// only CPC464: A exponent byte
// Exit: HL-target address AF|DE corrupt
#ifdef CPC_MODEL_464
#  define INTEGER_TO_REAL 0xBD40
#else
#  define INTEGER_TO_REAL 0xBD64
#endif
void firm_integer_to_real(U16 uInteger, U8* pDest) __sdcccall(1) {
  (void) uInteger;
  (void) pDest;

__asm
  push af
  res   7,  a
  call INTEGER_TO_REAL
  pop af
__endasm;
}
