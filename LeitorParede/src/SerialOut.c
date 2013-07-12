/*
 * SerialOut.c
 *
 * Created: 11/07/2013 17:12:31
 *  Author: ENGENHARIA2
 */ 

#include <asf.h>
#include "SerialOut.h"
#include "config_board.h"

#define BUFFERSIZE 10

static uint8_t _serial_tx_buff[BUFFERSIZE];

static inline void Monta_Pacote_Serial (uint64_t val)
{
	int8_t i;
	
	for(i=0; i<BUFFERSIZE; i++)
	{
		_serial_tx_buff[i]= (val % 16);
		
		if(_serial_tx_buff[i] < 10)
		{
			_serial_tx_buff[i] += '0';
		}
		else
		{
			_serial_tx_buff[i] = ((_serial_tx_buff[i] - 10) + 'A');
		}
		
		val /= 16;
	} 
}

void Transmite_Cartao_Serial(uint64_t val)
{
	int8_t i;
	
	Monta_Pacote_Serial(val);
	
	usart_putchar(USART_SERIAL,0x02);	// STX - Start of Text
	for(i=BUFFERSIZE-1;i>=0;i--)
	{
		usart_putchar(USART_SERIAL,_serial_tx_buff[i]);
	}
	usart_putchar(USART_SERIAL,0x0D);	// CR - Carriage Return
	usart_putchar(USART_SERIAL,0x0A);	// LF - Line Feed
	usart_putchar(USART_SERIAL,0x03);	// ETX - End of Text	
}

uint64_t Monta_Dados_Serial (uint64_t num, int ver)
{
	uint64_t card_data;
	
	card_data = (num | ((uint64_t)ver<<32));
	
	return card_data;
}
