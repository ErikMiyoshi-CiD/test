/*
 * PSK_Decode.h
 *
 * Created: 12/07/2013 14:44:25
 *  Author: Neto
 */ 


#ifndef FSK_DECODE_H_
#define FSK_DECODE_H_

#define FSK_SHORT_PERIOD_MIN	(2048-256)
#define FSK_SHORT_PERIOD_MAX	(2048+256)

#define FSK_LONG_PERIOD_MIN		(2560-256)
#define FSK_LONG_PERIOD_MAX		(2560+256)

void FSK_Decoding(void);

#endif /* PSK_DECODE_H_ */