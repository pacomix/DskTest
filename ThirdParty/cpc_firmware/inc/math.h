#ifndef CPC_FIRMWARE_MATH
#define CPC_FIRMWARE_MATH

#include "firmware.h"

void    firm_move_real          (U8* pDest, U8* pSrc)           __sdcccall(1);
void    firm_integer_to_real    (U16 uInteger, U8* pDest)       __sdcccall(1);
U16     firm_real_to_integer    (U8* pReal)                     __sdcccall(1);
void    firm_real_sub           (U8* pFirst, U8* pSecond)       __sdcccall(1);
void    firm_real_add           (U8* pFirst, U8* pSecond)       __sdcccall(1);
U16*    firm_real_int           (U8* uSrcReal)                  __sdcccall(1);
void    firm_real_multiplication(U8* pSrcReal, U8* pSecondReal) __sdcccall(1);
void    firm_real_division      (U8* pSrc, U8* pSecond)         __sdcccall(1);

#endif
