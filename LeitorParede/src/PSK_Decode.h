/*
 * PSK_Decode.h
 *
 * Created: 17/07/2013 17:59:06
 *  Author: ENGENHARIA2
 */ 


#ifndef PSK_DECODE_H_
#define PSK_DECODE_H_

#define EXPECTED_TRANS_AVG_MAX_TIME 480
#define EXPECTED_TRANS_AVG_MIN_TIME 450

#define TBIT_PULSE_CNT_MIN 11
#define TBIT_PULSE_CNT_MAX 14

#define SAMPLE_VECTOR_SIZE 4

void PSK_Decoding(void);

#endif /* PSK_DECODE_H_ */