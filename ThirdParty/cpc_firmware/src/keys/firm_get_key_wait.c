#include "types.h"

// Waits for a key to be pressed
// Carry TRUE | A character
#define KM_WAIT_KEY 0xBB18
U8 firm_get_key_wait(void) __sdcccall(1) {
__asm
  call KM_WAIT_KEY
__endasm;
}
