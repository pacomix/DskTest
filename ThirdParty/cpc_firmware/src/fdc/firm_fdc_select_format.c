#include "fdc.h"

// Action	Sets a format for a disc
// Entry	A holds the type of format that is to be selected
// Exit	AF, BC, DE and HL are corrupt, and all the other registers are preserved
// Notes	To select one of the normal disc formats, the following values should be put into the A register
// Data format - &C1
// System format - &41 - Used by CP/M
// IBM format - &01 - compatible with CP/M-86
// This routine sets the extended disc parameter block (XDPB) at &A890 to &A8A8
// - to set other formats, the XDPB must be altered directly
#define BIOS_SELECT_FORMAT 0xC039
void firm_fdc_select_format(U8 uFormat) __sdcccall(1) {
  (void) uFormat;
  __asm
    ld    hl, #_FIRM_FDC_SELECT_FORMAT
    ld    (__FAR_CALL+1), hl
    call  __FAR_CALL
    xor   a               ; set return code to true
__endasm;
}
