#ifndef CPC_FIRMWARE_TYPES
#define CPC_FIRMWARE_TYPES

typedef unsigned char U8;
typedef unsigned short int U16;
typedef signed char S8;
typedef signed short int S16;
typedef unsigned long int U32;
typedef signed long int I32;

typedef struct {
  U16 address;
  U8  rom;
} FARCALL;

#define false 0
#define true 1
#define bool U8
#define BOOL bool

#endif
