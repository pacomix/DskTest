#ifndef CPC_FIRMWARE_SCREEN
#define CPC_FIRMWARE_SCREEN

#include "firmware.h"

void    firm_set_screen_mode        (U8 mode)               __sdcccall(1);
void    firm_set_border_color       (U8 color1, U8 color2)  __sdcccall(1);
void    firm_set_palette_color      (U8 number, U16 color)  __sdcccall(1);
void    firm_wait_flyback           (void)                  __sdcccall(1);
void    firm_clear_palette_colors   (U8 border, U8 ink)     __sdcccall(1);
void    firm_set_palette_colors     (U8* palette)           __sdcccall(1);

#endif
