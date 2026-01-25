#include "txt.h"

// Resets the text VDU indirections and the control code table
// Exit: AF|BC|HL|DE corrupt
#define TXT_RESET 0xBB51
void firm_txt_reset(void) __sdcccall(1) {
__asm
  call TXT_RESET
__endasm;
}
