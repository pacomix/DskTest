#include "types.h"

// Gets the amount of interruptions happened since the machine is on or since the
// last time KL_TIME_SET was called.
// Returns DEHL
#define KL_TIME_PLEASE 0xBD0D
U16 firm_get_interruption_count(void) __sdcccall(1) {
__asm
  call KL_TIME_PLEASE
  ex    de, hl  ; __sdcccall(1) requires 16 bit values be put in DE, and the result
                ; is put in DEHL, hence we put the least significant bytes in DE
                ; and we discard the rest. Yes we are limited to 65535 ints.
                ; Thats a total of approx. 3.5 minutes counting.
__endasm;
}
