/*
 * AbaTK2.c
 *
 * Created: 10/07/2013 16:39:32
 *  Author: Administrator
 */ 
#include <asf.h>
#include "config_board.h"
#include "ABATK2.h"

#define TEMPO_ABATK2 120
#define END_SENTINEL 0b11111
#define START_SENTINEL 0b01011
#define NUM_DIGITS 14

static uint8_t _abatk2_buff[NUM_DIGITS+3]; // Numero de dados do cartao + SS + ES + LRC 

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

static inline void Enviar_Caracter_ABA_TK2(uint8_t dado)
{
	int8_t i;
	// Lembrar dos transitores de saida que invertem os dados
	
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

uint64_t Monta_Dados_Cartao_ABATK2(uint64_t num, int ver)
{
	uint64_t abatk2_num;
	
	abatk2_num = (num | ((uint64_t)ver<<32));
	
	return abatk2_num;
}


