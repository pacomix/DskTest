#include "fdc.h"

// Action	Enables or disables disc error messages
// Entry	To enable messages, A holds &00; to disable messages, A holds &FF
// Exit	A holds the previous state, HL and the flags are corrupt, and all others are preserved
// Notes	Enabling and disabling the messages can also be achieved by poking &BE78 with &00 or &FF
#define BIOS_SET_MESSAGE 0xC033
void firm_fdc_messages(U8 uVal) __sdcccall(1) {
  (void) uVal;
__asm
  ld    hl, #_FIRM_FDC_SET_MESSAGE
  ld    (__FAR_CALL+1), hl
  call  __FAR_CALL
__endasm;
}
