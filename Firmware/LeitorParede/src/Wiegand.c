/*
 * Wiegand.c
 *
 * Created: 11/07/2013 09:49:38
 *  Author: Administrator
 */ 

#include <asf.h>
#include "config_board.h"
#include "Wiegand.h"

#define WIE_NUMDIGITS 24  		// Número de bits de dados no frame Wiegand a ser transmitido
#define DATA_PULSE_TIME	100		// Intervalo de tempo em que o pulso de dados é mantido em nível baixo - Unidade: microsegundos
#define DATA_INTERVAL_TIME 2000	// Intervalo de tempo entre os pulsos de dados - Unidade: milisegundos

static inline uint8_t Calcula_Paridade(uint16_t val)
{
	
	uint16_t tmp = val;
	uint8_t parity = 0;
	
	while (tmp)
	{
		parity ^= tmp & 0x1;
		tmp >>= 1;
	}
	
	return parity; // Paridade impar
}

static inline uint32_t Codifica_Wiegand(uint32_t val)
{
	
	uint32_t coded_message;
	uint32_t site_parity;
	uint8_t card_parity;
	
	site_parity = Calcula_Paridade( (uint16_t)( (val >> 16) & 0xFF) ) ;	// Paridade par
	card_parity = !Calcula_Paridade( (uint16_t)( val & 0xFFFF) );		// Paridade impar
	
	coded_message = ( (site_parity << (WIE_NUMDIGITS+1)) | (val << 1) | card_parity );
	
	return coded_message;	
}

static inline void Envia_Payload(uint32_t mensagem)
{
	uint32_t tmp;
	int8_t i;
	
	for(i=WIE_NUMDIGITS+1; i>=0;i--)
	{
		tmp = (mensagem & (1l<<25));
		
		if(tmp)
		{
			ioport_set_pin_level(D1_DATA,1); // Transitor de saida inverte o sinal
			delay_us(DATA_PULSE_TIME);
			ioport_set_pin_level(D1_DATA,0);
		}
		else
		{
			ioport_set_pin_level(USART_TX_PIN,1);
			delay_us(DATA_PULSE_TIME);
			ioport_set_pin_level(USART_TX_PIN,0);
		}
		
		delay_us(DATA_INTERVAL_TIME);
		mensagem <<= 1;
	}
}

void Transmite_Pacote_Wiegand(uint32_t card_num)
{
	uint32_t transmit_message;
	
	transmit_message = Codifica_Wiegand(card_num);
	Envia_Payload(transmit_message);
}

