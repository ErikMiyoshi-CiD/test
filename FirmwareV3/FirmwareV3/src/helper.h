#include <asf.h>

#include "RF_common.h"

#ifndef HELPER_H_
#define HELPER_H_

#define USER_INFO_ADD 0x3F00

#define USER_INFO_POS_RFID 0
#define USER_INFO_POS_OUTP 1

#define USER_INFO_WIE_OUTP 'W'
#define USER_INFO_ABA_OUTP 'A'

#define USER_INFO_ASK_RFID 'A'
#define USER_INFO_FSK_RFID 'F'
#define USER_INFO_PSK_RFID 'P'
#define USER_INFO_MIF_RFID 'M'

typedef enum {
	OUTPUT_WIEGAND,
	OUTPUT_ABATRACK
} TIPO_OUTPUT;

typedef enum {
	MODO_PROGRAMACAO,
	MODO_NORMAL	
} MODO_LEITOR;

extern TIPO_OUTPUT tipo_output;

TIPO_LEITOR ler_tipo_leitor(void);
MODO_LEITOR avaliar_modo_leitor(void);
void user_init(void);
void modo_leitor(void);
void Init125khz(void);
void buzzer_clock_init(void);
void buzz(uint32_t);
void buzz_on(void);
void buzz_off(void);
void ok_feedback(void);
void led_idle(void);
void led_green(void);
void led_red(void);
void led_yellow(void);
void led_off(void);
void programa_config (uint16_t dados);

#endif
