#ifndef CPC_FIRMWARE_SYS
#define CPC_FIRMWARE_SYS

#include "firmware.h"

bool    firm_kl_find_cmd            (U8* uCmd, void* pStorage)  __sdcccall(1);
U16     firm_get_interruption_count (void)                      __sdcccall(1);
void    firm_set_interruption_count (U16 lsb, U16 msb)          __sdcccall(1);

#endif
