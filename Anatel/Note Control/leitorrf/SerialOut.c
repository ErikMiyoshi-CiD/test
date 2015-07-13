/*
 * SerialOut.c
 *
 * Created: 14/11/2013 10:39:19
 *  Author: Neto, Luiz
 */ 

#include "sam.h"
#include "helper.h"
#define BUFFERSIZE 10	// Tamanho do buffer que armazena os dados do cart�o em formato ASCII representando os caracteres hexadecimais

static uint8_t _serial_tx_buff[BUFFERSIZE]; // Buffer de armazenamento de informa��o

// Recebe os dados de informa��o do cart�o e converte os valores binarios em caracteres ASCII hexadecimais preenchendo o buffer

void usart_putchar(char caracter){
	//TODO
}

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

// Envia os dados do cart�o lido atrav�s da interface serial (USART/RS-232) do processador. Caracteres constantes (STX, ETX, etc) n�o
// s�o colocados no buffer e s�o enviados separadamente.

void Transmite_Cartao_Serial(uint64_t val)
{
	int8_t i;
	
	Monta_Pacote_Serial(val);
	
	usart_putchar(0x02);	// STX - Start of Text
	for(i=BUFFERSIZE-1;i>=0;i--)
	{
		usart_putchar(_serial_tx_buff[i]);
	}
	usart_putchar(0x0D);	// CR - Carriage Return
	usart_putchar(0x0A);	// LF - Line Feed
	usart_putchar(0x03);	// ETX - End of Text
}

//Recebe os dados de site e facility do cart�o RFID e concatena em uma �nica vari�vel para
//tratamento pelas rotinas de envio. Caso o dado adquirido j� possua as informa��es concatenadas
//e sem paridade n�o � necess�rio utilizar esta fun��o.

uint64_t Monta_Dados_Serial (uint64_t num, int ver)
{
	uint64_t card_data;
	
	card_data = (num | ((uint64_t)ver<<32));
	
	return card_data;
}