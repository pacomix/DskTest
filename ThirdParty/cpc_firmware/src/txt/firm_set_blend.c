#include "txt.h"

// Action	Sets the character write mode to either opaque or transparent
// Entry	For transparent mode, A must be non-zero; for opaque mode, A has to hold zero
// Exit	AF and HL are corrupt, and all other registers are preserved
// Notes	Setting the character write mode has no effects on the graphics VDU
#define TXT_SET_BACK 0xBB9F
void firm_set_blend(bool bBlend) __sdcccall(1) {
  (void) bBlend;

__asm
  call  TXT_SET_BACK
__endasm;
}
