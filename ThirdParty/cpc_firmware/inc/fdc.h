
#ifndef CPC_FIRMWARE_FDC
#define CPC_FIRMWARE_FDC

#include "firmware.h"

/*
_FIRM_FDC_SET_MESSAGE::     .dw 0xC033
                            .db 0x07
_FIRM_FDC_SET_UP_DISC::     .dw 0xC036
                            .db 0x07
_FIRM_FDC_SELECT_FORMAT::   .dw 0xC039
                            .db 0x07
_FIRM_FDC_READ_SECTOR::     .dw 0xC03C
                            .db 0x07
_FIRM_FDC_WRITE_SECTOR::    .dw 0xC03F
                            .db 0x07
_FIRM_FDC_FORMAT_TRACK::    .dw 0xC042
                            .db 0x07
_FIRM_FDC_MOVE_TRACK::      .dw 0xC045
                            .db 0x07
_FIRM_FDC_GET_DR_STATUS::   .dw 0xC048
                            .db 0x07
_FIRM_FDC_SET_RETRY_COUNT:: .dw 0xC04B
                            .db 0x07
                            */

extern FARCALL FIRM_FDC_SET_MESSAGE;
extern FARCALL FIRM_FDC_SET_UP_DISC;
extern FARCALL FIRM_FDC_SELECT_FORMAT;
extern FARCALL FIRM_FDC_READ_SECTOR;
extern FARCALL FIRM_FDC_WRITE_SECTOR;
extern FARCALL FIRM_FDC_FORMAT_TRACK;
extern FARCALL FIRM_FDC_MOVE_TRACK;
extern FARCALL FIRM_FDC_GET_DR_STATUS;
extern FARCALL FIRM_FDC_SET_RETRY_COUNT;

bool    firm_fdc_init_routines  (void)                                              __sdcccall(1);
bool    firm_fdc_goto_track     (U8 drive_number, U8 track)                         __sdcccall(1);
U8      firm_fdc_read_sector    (U8* target_buffer, U8 track, U8 drive, U8 sector)  __sdcccall(0);
void    firm_fdc_messages       (U8 uVal)                                           __sdcccall(1);
void    firm_fdc_setup          (void)                                              __sdcccall(1) __naked;
void    my_far_cry              (void)                                              __sdcccall(1) __naked;
void    firm_fdc_select_format  (U8 uFormat)                                        __sdcccall(1);
bool    firm_fdc_get_status     (U8 drive)                                          __sdcccall(1);

#endif
