#include "fdc.h"

void my_far_cry(void) __sdcccall(1) __naked {
__asm
__FAR_CALL: rst 24
            .dw 0x0000
            ret
__endasm;
}
