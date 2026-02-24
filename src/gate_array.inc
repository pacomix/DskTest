#define GA_REG_SELECT_BORDER  0b00010000
#define GA_REG_SELECT_PEN     0b00000000  // Specify pen number (0-15) with GA_REG_SELECT_PEN | number
#define GA_REG_SET_COLOR      0b01000000  // Specify color number (0-31) with GA_REG_SET_COLOR | number
#define GA_REG_SET_COLOR_MASK 0b01011111

#define GA_REG_SELECT_PEN0  0b00000000
#define GA_REG_SELECT_PEN1  0b00000001
#define GA_REG_SELECT_PEN2  0b00000010
#define GA_REG_SELECT_PEN3  0b00000011
#define GA_REG_SELECT_PEN4  0b00000100
#define GA_REG_SELECT_PEN5  0b00000101
#define GA_REG_SELECT_PEN6  0b00000110
#define GA_REG_SELECT_PEN7  0b00000111
#define GA_REG_SELECT_PEN8  0b00001000
#define GA_REG_SELECT_PEN9  0b00001001
#define GA_REG_SELECT_PEN10 0b00001010
#define GA_REG_SELECT_PEN11 0b00001011
#define GA_REG_SELECT_PEN12 0b00001100
#define GA_REG_SELECT_PEN13 0b00001101
#define GA_REG_SELECT_PEN14 0b00001110
#define GA_REG_SELECT_PEN15 0b00001111

#define GA_REG_SET_COLOR0   0b01000000
#define GA_REG_SET_COLOR1   0b01000001
#define GA_REG_SET_COLOR2   0b01000010
#define GA_REG_SET_COLOR3   0b01000011
#define GA_REG_SET_COLOR4   0b01000100
#define GA_REG_SET_COLOR5   0b01000101
#define GA_REG_SET_COLOR6   0b01000110
#define GA_REG_SET_COLOR7   0b01000111
#define GA_REG_SET_COLOR8   0b01001000
#define GA_REG_SET_COLOR9   0b01001001
#define GA_REG_SET_COLOR10  0b01001010
#define GA_REG_SET_COLOR11  0b01001011
#define GA_REG_SET_COLOR12  0b01001100
#define GA_REG_SET_COLOR13  0b01001101
#define GA_REG_SET_COLOR14  0b01001110
#define GA_REG_SET_COLOR15  0b01001111
#define GA_REG_SET_COLOR16  0b01010000
#define GA_REG_SET_COLOR17  0b01010001
#define GA_REG_SET_COLOR18  0b01010010
#define GA_REG_SET_COLOR19  0b01010011
#define GA_REG_SET_COLOR20  0b01010100
#define GA_REG_SET_COLOR21  0b01010101
#define GA_REG_SET_COLOR22  0b01010110
#define GA_REG_SET_COLOR23  0b01010111
#define GA_REG_SET_COLOR24  0b01011000
#define GA_REG_SET_COLOR25  0b01011001
#define GA_REG_SET_COLOR26  0b01011010

// __sfr __banked __at(0x7F00) ga_port;  // WRITE ONLY
__sfr __banked __at(0b0100000000000000) ga_port;  // WRITE ONLY
__sfr __banked __at(0b0000000100000000) fdc_status_port;  // WRITE ONLY
//                  0bxxxxx0x10xxxxxx0
__sfr __banked __at(0b0100000100000000) ga_fdc_port;  // WRITE ONLY
__sfr __banked __at(0b0111100100010000) ga_border_and_fdc_port;  // WRITE ONLY / READ FDC
__sfr __banked __at(0b0111100100010001) ga_border_and_fdc_port_data;  // WRITE ONLY / READ FDC

//01xxxxxx xxxxxxxx

extern U8 g_ga_color_border;

void ga_variables(void) __naked {
__asm
  _g_ga_color_border:   .db GA_REG_SET_COLOR0  ; Border color specified in hardware color format
__endasm;
}

void ga_set_color_border(void) {
  ga_port = GA_REG_SELECT_BORDER;
  ga_port = g_ga_color_border & 0b01011111;
}
