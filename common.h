#ifndef COMMON_H
#define COMMON_H

#define bname(s) bstr(s)
#define bstr(s) b##s

typedef struct {
  uint8_t b0:1;
  uint8_t b1:1;
  uint8_t b2:1;
  uint8_t b3:1;
  uint8_t b4:1;
  uint8_t b5:1;
  uint8_t b6:1;
  uint8_t b7:1;
} _bitmask;

#define BITPOINTER(register,bit) ((volatile _bitmask*)&register)->bname(bit)

#define SET(x,y)    (x |= (1 << y))
#define CLR(x,y)    (x &= ~(1 << y))

#define ON				1
#define OUT				1
#define HIGH			1
#define IN 				0
#define LOW				0
#define OFF				0
#define RESET_INT_FLAG	1

#endif
