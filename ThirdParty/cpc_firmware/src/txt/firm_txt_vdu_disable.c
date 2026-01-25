#include "txt.h"

#define TXT_VDU_DISABLE 0xBB57
void firm_txt_vdu_disable(void) __sdcccall(1) {
__asm
  call TXT_VDU_DISABLE
__endasm;
}
