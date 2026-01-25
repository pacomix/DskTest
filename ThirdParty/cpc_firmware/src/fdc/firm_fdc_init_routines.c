#include "fdc.h"

bool firm_fdc_init_routines(void) __sdcccall(1) {
  U8 uRSXCMD = 0x81;
  FARCALL* pDst = &FIRM_FDC_SET_MESSAGE;

  do {
    if (!firm_kl_find_cmd(&uRSXCMD, pDst)) {
      return false;
    }
  } while(++pDst, ++uRSXCMD < 0x8A);
  return true;
}
