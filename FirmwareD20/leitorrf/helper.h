/*
 * helper.h
 *
 * Created: 08/11/2013 18:27:55
 *  Author: Luiz
 */ 

#include "sam.h"

#ifndef HELPER_H_
#define HELPER_H_

void ioport_set_pin_level(uint8_t pino, int estado);
void delay_us(volatile int tempo);
/* static inline */void system_flash_set_waitstates(uint8_t wait_states);
/* static inline */int system_gclk_is_syncing(void);
void ClockInit();
void Init125khz(void);
void buzzer_clock_init(void);
void buzz(void);
void ok_feedback(void);
void system_init(void);


#endif

#ifndef GREEN_LED
#define GREEN_LED		3
#endif

#ifndef RED_LED
#define RED_LED			4
#endif

#ifndef D1_DATA
#define D1_DATA			9
#endif

#ifndef USART_TX_PIN
#define USART_TX_PIN	8
#endif

#ifndef CARD_PRES
#define CARD_PRES		10
#endif

#ifndef BUZZER
#define BUZZER			25
#endif

#ifndef USART_SERIAL
#define USART_SERIAL	8
#endif