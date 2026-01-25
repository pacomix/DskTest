#include "types.h"

#define false 0
#define true 1
#define bool U8
#define BOOL bool

#define OFF false
#define ON true
#define FALSE false
#define TRUE true
#define bool U8
#define BOOL bool
#define MOTOR_OFF OFF
#define MOTOR_ON ON

// Set the graphic mode
// TODO - Implement and TEST functionality when loaded from a CPC464. Distribute TWO .dsks
#define CPC_MODEL_6128
#define CPC_MODE2

# if defined (CPC_MODE0)
#   define CPC_SCR_MODE 0
#   define CPC_SCR_CHARS_WIDTH 40

# elif defined (CPC_MODE1)
#   define CPC_SCR_MODE 1
#   define CPC_SCR_CHARS_WIDTH 40

# elif defined (CPC_MODE2)
#   define CPC_SCR_MODE 2
#   define CPC_SCR_CHARS_WIDTH 80

# else
#   error Graphic mode not defined!!!
#endif