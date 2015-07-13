/*
 * RF_ASK.h
 *
 * Created: 2014-02-25 4:34:13 PM
 *  Author: Alb
 */ 


#ifndef RF_ASK_H_
#define RF_ASK_H_

#define ASK_BUF_SIZE 128

typedef enum
{
	IDLE,
	SYNCHING,
	DEMOD_TRANS,
	DEMOD_WAIT
} MANCHESTER_STATE;

void ASK_Init(void);
void ASK_Run(void);

#endif /* RF_ASK_H_ */