#ifndef CPC_FIRMWARE
#define CPC_FIRMWARE

#if !defined (CPC_MODEL_464) && !defined(CPC_MODEL_6128)
#   error "Platform undefined!!! Please define CPC_MODEL_XXX directive to target 464 or 6128."
#endif

#include "types.h"

#include "sys.h"
#include "fdc.h"
#include "keys.h"
#include "math.h"
#include "screen.h"
#include "txt.h"

#endif
