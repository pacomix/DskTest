#include "types.h"

// Copy a real (float) value to another place.
// HL - Target address where the value will be copied.
// DE - Source address where the value is stored (5 bytes)
// only CPC464: A exponent byte
// Exit: HL-target address Carry TRUE F-corrupt
#ifdef CPC_MODEL_464
#  define MOVE_REAL 0xBD3D
#else
#  define MOVE_REAL 0xBDC1
#endif
void firm_move_real(U8* pDest, U8* pSrc) __sdcccall(1) {
  (void) pDest;
  (void) pSrc;

__asm
  call MOVE_REAL
__endasm;
}
