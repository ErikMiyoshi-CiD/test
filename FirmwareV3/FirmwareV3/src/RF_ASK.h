#ifndef RF_ASK_H_
#define RF_ASK_H_

#define ASK_PERIODS 2
#define ASK_BUF_COUNT 2
#define ASK_BUF_SIZE 512

typedef enum
{
	IDLE,
	SYNCHING,
	DEMOD_TRANS,
	DEMOD_WAIT
} MANCHESTER_STATE;

void ASK_Init(void);
void ASK_Run(void);

void Init_Timer(void);

#endif /* RF_ASK_H_ */