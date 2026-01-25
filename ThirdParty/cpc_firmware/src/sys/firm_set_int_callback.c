#include "types.h"

// Set routine to be called for every interrupt (1/300)
// HL - Jump address
// AF/DE/HL corrupted afterwards
#define KL_NEW_FAST_TICKER 0xBCE0
#define KL_DEL_FAST_TICKER 0xBCE6
void firm_set_int_callback(U8* pFn) __sdcccall(1) {
  (void)pFn;
__asm
  call KL_DEL_FAST_TICKER
__endasm;
}
