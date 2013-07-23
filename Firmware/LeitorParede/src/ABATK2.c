/*
 * AbaTK2.c
 *
 * Created: 10/07/2013 16:39:32
 *  Author: Neto & David
 */ 
#include <asf.h>
#include "config_board.h"
#include "ABATK2.h"

#define TEMPO_ABATK2 120		// Tempo em que um pulso é mantido em nível baixo
#define END_SENTINEL 0b11111	// Campo END SENTINEL do frame de mensagem ABATK2
#define START_SENTINEL 0b01011	// Campo START SENTINEL do frame de mensagem ABATK2
#define NUM_DIGITS 14			// Número de campos do frame que efetivamente recebem informações do cartão

static uint8_t _abatk2_buff[NUM_DIGITS+3]; // Numero de dados do cartao + SS + ES + LRC 

// Recebe 4 bits de dados e retorna o mesmo valor com a paridade concatenada.

static inline uint8_t encode_with_parity(uint8_t val)
{
	uint8_t tmp = val;
	uint8_t parity = 0;
	
	while (tmp)
	{
		parity ^= tmp & 0x1;
		tmp >>= 1;
	}
	
	return ((val & 0xf) | ((!parity)<<4));
}

//Rotina de envio de frame de 5 bits via protocolo ABATK2.

static inline void Enviar_Caracter_ABA_TK2(uint8_t dado)
{
	int8_t i;
		
	for(i=4;i>=0;i--)
	{
		ioport_set_pin_level(D1_DATA,!(dado & 1));
		delay_us(TEMPO_ABATK2);
		ioport_set_pin_level(USART_TX_PIN,1);
		delay_us(TEMPO_ABATK2);
		ioport_set_pin_level(USART_TX_PIN,0);
		dado >>= 1;
	}
}

//Calcula o campo LRC utilizado na transmissão via ABATK2.

static inline uint8_t Calcula_LRC(void)
{
	int8_t i=0;
	uint8_t lrc=0;
	
	for(i=NUM_DIGITS+1;i>=0;i--)
	{
		lrc ^= _abatk2_buff[i];
	}
	
	return encode_with_parity(lrc);
}

//Recebe os dados do cartão com site e facility code e monta o buffer com os dados a serem enviados.
//Já calula paridade a cada 4 bit e o campo de LRC.

static inline void Codifica_ABATK2(uint64_t num)
{
	
	uint8_t i;
	
	_abatk2_buff[0]=START_SENTINEL; // Start
	_abatk2_buff[NUM_DIGITS+1]=END_SENTINEL; // End sentinel
	
	for(i=NUM_DIGITS;i>=1;i--)
	{
		_abatk2_buff[i] = encode_with_parity(num % 10);
		num /= 10;
	}
	
	_abatk2_buff[NUM_DIGITS+2]=Calcula_LRC(); //LRC
	
}

void Enviar_ABA_TK2(uint64_t val) // Recebe o valor do cartao
{
	int8_t i;
	
	Codifica_ABATK2(val);
	
	ioport_set_pin_level(CARD_PRES,1);
	delay_us(20);
	Enviar_Caracter_ABA_TK2(0);
	Enviar_Caracter_ABA_TK2(0);
	for(i=0;i<sizeof(_abatk2_buff);i++)
	{
		Enviar_Caracter_ABA_TK2(_abatk2_buff[i]);
	}
	Enviar_Caracter_ABA_TK2(0);
	Enviar_Caracter_ABA_TK2(0);
	ioport_set_pin_level(CARD_PRES,0);
}

//Recebe os dados de site e facility do cartão RFID e concatena em uma única variável para
//tratamento pelas rotinas de envio. Caso o dado adquirido já possua as informações concatenadas
//e sem paridade não é necessário utilizar esta função.

uint64_t Monta_Dados_Cartao_ABATK2(uint64_t num, int ver)
{
	uint64_t abatk2_num;
	
	abatk2_num = (num | ((uint64_t)ver<<32));
	
	return abatk2_num;
}


