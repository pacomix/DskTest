#ifndef CPC_FIRMWARE_TXT
#define CPC_FIRMWARE_TXT

#include "firmware.h"

void    firm_txt_init           (void)                              __sdcccall(1);
void    firm_txt_reset          (void)                              __sdcccall(1);
void    firm_txt_vdu_enable     (void)                              __sdcccall(1);
void    firm_txt_vdu_disable    (void)                              __sdcccall(1);
void    firm_put_char           (U8 character)                      __sdcccall(1);
void    firm_put_char_at_cursor (U8 character)                      __sdcccall(1);
void    firm_put_char_at        (U8 character, U8 line, U8 column)  __sdcccall(1);
void    firm_set_cursor_at      (U8 column, U8 line)                __sdcccall(1);
void    firm_set_cursor_at_x    (U8 column)                         __sdcccall(1);
void    firm_set_cursor_at_y    (U8 row)                            __sdcccall(1);
void    firm_set_inverse        (void)                              __sdcccall(1);
void    firm_set_blend          (bool bBlend)                       __sdcccall(1);

#endif
