#include "types.h"

// Converts IN PLACE an integer value into real (float)
// HL - Source address where the value is stored (5 bytes)
// Exit: HL-target address of the INT value (4 byte) B b7-sign bit
//   AF|B|IX corrupt
#ifdef CPC_MODEL_464
#  define REAL_INT 0xBD4F
#else
#  define REAL_INT 0xBD73
#endif
U16* firm_real_int(U8* uSrcReal) __sdcccall(1) {
  (void) uSrcReal;

__asm
  call REAL_INT
  ex  de, hl
  inc de
  inc de
__endasm;
}
