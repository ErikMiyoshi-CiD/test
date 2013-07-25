/*
 * ASK_Decode.h
 *
 * Created: 04/07/2013 19:35:22
 *  Author: Neto
 */ 

#ifndef ASK_DECODE_H_
#define ASK_DECODE_H_

#define GLITCH_TIME 1000u

#define MIN_SHORT_TIME 4992u   // 156us
#define MAX_SHORT_TIME 11392u  // 356us 

#define MIN_LONG_TIME 13184u   // 412us
#define MAX_LONG_TIME 19584u   // 612us

#define COL0	0x0008421084210842ull

void ASK_Decoding(void);


#endif /* ASK_DECODE_H_ */