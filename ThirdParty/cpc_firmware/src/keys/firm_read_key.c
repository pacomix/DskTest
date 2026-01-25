#include "types.h"
// Read key without waiting
// Carry TRUE | A character
#define KM_READ_KEY 0xBB1B
U8 firm_read_key(void) __sdcccall(1) {
__asm
  call KM_READ_KEY
  jp c, firm_get_key_wait_end
  xor a
firm_get_key_wait_end:
__endasm;
}
