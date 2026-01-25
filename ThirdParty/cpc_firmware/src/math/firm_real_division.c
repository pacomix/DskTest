#include "types.h"

// Divides IN PLACE of the first number two real (float) values
// HL - Source address of the first real and where the value is stored (5 bytes)
// DE - Source address of the second real (float) value. Unaltered. (5 bytes)
// Exit: AF|BC|DE|IX|IY corrupt
#ifdef CPC_MODEL_464
#  define REAL_DIVISION 0xBD64
#else
#  define REAL_DIVISION 0xBD88
#endif
void firm_real_division(U8* pSrc, U8* pSecond) __sdcccall(1) {
  (void) pSrc;
  (void) pSecond;

__asm
  call REAL_DIVISION
__endasm;
}
// TODO - Cambiar esto. Usar la jumpblock en lugar de las direcciones directas al firm...
