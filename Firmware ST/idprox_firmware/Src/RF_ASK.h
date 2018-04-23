#ifndef RF_ASK_H_
#define RF_ASK_H_

#define ASK_PERIODS 2
#define ASK_BUF_COUNT 2
#define ASK_BUF_SIZE 512

// Externaliza variáveis para o callback handler em "tim.c"
extern volatile uint8_t cur_buf;
extern volatile uint16_t n_samples;
extern volatile uint8_t semaforo_ask;

extern uint8_t buf[ASK_BUF_COUNT][ASK_PERIODS][ASK_BUF_SIZE];

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