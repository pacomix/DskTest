#include "txt.h"

/* TEXT FUNCTIONS */
// Initializes the TEXT VDU whatever that be
// Exit: AF|BC|HL|DE corrupt
#define TXT_INITIALIZE 0xBB4E
void firm_txt_init(void) __sdcccall(1) {
__asm
  call TXT_INITIALIZE
__endasm;
}
