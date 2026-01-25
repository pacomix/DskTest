#include "types.h"

// Wait for flyback (once the beam has finished drawing the screen to monitor)
#define MC_WAIT_FLYBACK 0xBD19
void firm_wait_flyback(void) __sdcccall(1) {
__asm
  call MC_WAIT_FLYBACK
__endasm;
}
