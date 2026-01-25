#include "types.h"

// Set the interrupt count.
// DEHL - interrupt count to set
// AF corrupt
#define KL_TIME_SET 0xBD10
void firm_set_interruption_count(U16 lsb, U16 msb) __sdcccall(1) {
  (void)lsb;
  (void)msb;
__asm
  call KL_TIME_SET
__endasm;
}
