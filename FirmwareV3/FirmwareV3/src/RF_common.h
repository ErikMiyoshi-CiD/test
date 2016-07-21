#ifndef COMMON_H_
#define COMMON_H_

#define BUF_SIZE	128
#define BUF_COUNT	2

typedef enum {
	TIPO_ASK,
	TIPO_FSK,
	TIPO_PSK,
	TIPO_MIFARE	
} TIPO_LEITOR;

extern uint8_t buf[BUF_COUNT][BUF_SIZE];

#endif /* COMMON_H_ */