#include "txt.h"

// Allow chars to be printed on the screen in the current stream
// Exit: AF corrupt
#define TXT_VDU_ENABLE 0xBB54
void firm_txt_vdu_enable(void) __sdcccall(1) {
__asm
  call TXT_VDU_ENABLE
__endasm;
}
