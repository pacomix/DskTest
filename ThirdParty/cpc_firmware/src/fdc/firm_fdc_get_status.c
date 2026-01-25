#include "fdc.h"

// Return status of target drive
// A - Drive number
// Exit:
//   OK   - Carry TRUE A=Status b6-write protect or disc missing b5-ready with
// disc b4-head in track0
//   FAIL - Carry FALSE A=corrupt HL-Address of status byte-1
#define BIOS_GET_STATUS 0xC048
bool firm_fdc_get_status(U8 drive) __sdcccall(1) {
  (void) drive;
__asm
  ld    hl, #_FIRM_FDC_GET_DR_STATUS
  ld    (__FAR_CALL+1), hl
  call  __FAR_CALL
  jr    c, __firm_fdc_get_status_get_result
  inc   hl
  ld    a, (hl)

__firm_fdc_get_status_get_result:
__endasm;
}
