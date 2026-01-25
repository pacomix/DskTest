#include "types.h"

// Multiplies IN PLACE of the first number two real (float) values
// HL - Source address of the first real and where the value is stored (5 bytes)
// DE - Source address of the second real (float) value. Unaltered. (5 bytes)
// Exit: AF|BC|DE|IX|IY corrupt
#ifdef CPC_MODEL_464
#  define REAL_MULTIPLICATION 0xBD61
#else
#  define REAL_MULTIPLICATION 0xBD85
#endif
void firm_real_multiplication(U8* pSrcReal, U8* pSecondReal) __sdcccall(1) {
  (void) pSrcReal;
  (void) pSecondReal;

__asm
  call REAL_MULTIPLICATION
__endasm;
}
