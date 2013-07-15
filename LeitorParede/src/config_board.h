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

#define LED_GREEN (IOPORT_CREATE_PIN(PORTD,5))		// Pino de saída para controle do LED Verde de acesso
#define BUZZER (IOPORT_CREATE_PIN(PORTD,3))			// Pino de saída para geração de frequência do Buzzer
#define RELAY (IOPORT_CREATE_PIN(PORTC,4))			// Pino de saída para controle da bobina do relé
#define RFID_CLK (IOPORT_CREATE_PIN(PORTC,3))		// Pino de saída para geração do clock de 125kHz excitador da antena
#define ASK_FSK (IOPORT_CREATE_PIN(PORTD,2))		// Pino de entrada para recepção do sinal filtrado
#define CARD_PRES (IOPORT_CREATE_PIN(PORTR,1))		// Pino de saída para sinalização de presença de cartão no protocolo ABATK2 (Card Present)
#define USART_TX_PIN IOPORT_CREATE_PIN(PORTD, 7)	// Pino de saída para envio dos dados de Wiegand (D0), TX (Serial) e CLK (ABATK2).
#define D1_DATA IOPORT_CREATE_PIN(PORTC,0)			// Pino de saída para envio dos dados de Wiegand (D1)
#define LED_IN IOPORT_CREATE_PIN(PORTC,7)			// Pino de entrada para sinalização de que o cartão foi aceito e a entrada foi liberada

#define USART_SERIAL               &USARTD0					// Serial utilizada para transmistir
#define USART_SERIAL_BAUDRATE      115200					// Velocidade de transmissao
#define USART_SERIAL_LENGTH        USART_CHSIZE_8BIT_gc		// Tamanho do frame de dados
#define USART_SERIAL_PARITY        USART_PMODE_DISABLED_gc	// Sem paridade
#define USART_SERIAL_STOP_BIT      0						// 1 bit de parada

void Inicializa_USART(void);

void Inicializa_Placa(void);

void Liga_Buzzer(uint16_t period);
void Conf_Buzzer(void);
void Desliga_Buzzer(void);

void Liga_125kHz(void);

void Inicializa_GPIO(void);
void ASK_Pin_Config(void);
void FSK_Pin_Config(void);
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