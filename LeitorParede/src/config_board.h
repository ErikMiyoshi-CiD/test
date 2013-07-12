/*
 * config_board.h
 *
 * Created: 02/07/2013 10:21:04
 *  Author: Neto
 */ 
#include <asf.h>

#ifndef CONFIG_BOARD_H_
#define CONFIG_BOARD_H_

#define INTMASK2 2 // Pino da PORTD que receberá a interrupção

#define INT2IF 2 // Flag da interrupção do pino de ASK_FSK

#define LED_GREEN (IOPORT_CREATE_PIN(PORTD,5))
#define BUZZER (IOPORT_CREATE_PIN(PORTD,3))
#define RELAY (IOPORT_CREATE_PIN(PORTC,4))
#define RFID_CLK (IOPORT_CREATE_PIN(PORTC,3))
#define ASK_FSK (IOPORT_CREATE_PIN(PORTD,2))
#define CARD_PRES (IOPORT_CREATE_PIN(PORTR,1))
#define USART_TX_PIN IOPORT_CREATE_PIN(PORTD, 7)
#define D1_DATA IOPORT_CREATE_PIN(PORTC,0)
#define LED_IN IOPORT_CREATE_PIN(PORTC,7)

#define USART_SERIAL               &USARTD0
#define USART_SERIAL_BAUDRATE      115200
#define USART_SERIAL_LENGTH        USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY        USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT      0

void Inicializa_USART(void);

void Inicializa_Placa(void);

void Liga_Buzzer(uint16_t period);
void Conf_Buzzer(void);
void Desliga_Buzzer(void);

void Liga_125kHz(void);

void Inicializa_GPIO(void);
void ASK_FSK_Pin_Config(void);
void Clear_PORTD_Int_Flag(void);

void Liga_Timer_XCL_16bits(void);
void Desliga_Timer_XCL_16bits(void);
void Conf_Timer_XCL_16bits(void);
uint16_t  Cont_Timer_XCL_16bits(void);

void Liga_TC5(void);
void Desliga_TC5(void);
void Configura_TC5(void);
uint16_t Ler_Contagem_TC5(void);
void Reinicia_Contagem_TC5(void);
void TC5_Set_CountUp_Mode(void);

#endif /* CONFIG_BOARD_H_ */