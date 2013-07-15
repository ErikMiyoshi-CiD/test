/*
 * FSK_Decode.c
 *
 * Created: 12/07/2013 14:44:07
 *  Author: Neto
 */ 

#include <asf.h>
#include "FSK_Decode.h"
#include "config_board.h"
#include "string.h"
#include <stdio.h>

static uint64_t _val;
static int _last_bit;
static int _num_bits;
static int _valendo;

static void comeca_leitura(void)
{
	ioport_set_pin_level(CARD_PRES,1);
	_val = 0;
	_num_bits = 0;
	_valendo = 1;
}

static void aborta_leitura(void)
{
	_valendo = 0;
	ioport_set_pin_level(CARD_PRES,0);
}

static inline void leu_bit(int bit)
{
	if (!_valendo)
		return;
	
	if (_num_bits % 2 == 1)
	{
		_val <<= 1;
		
		if (_last_bit && !bit)
		{
			// Transicao alto --> baixo
			_val |= 1;
		}
		else if (!_last_bit && bit)
		{
			// Transicao baixo --> alto
			//_val |= 0;
		}
		else
		{
			aborta_leitura();
			return;
		} 

	}
	_last_bit = bit;
	++_num_bits;

	if (_num_bits / 2 >= 45)
	{
		uint8_t Hex_Num [17];
		// Acabou!
		aborta_leitura();
		sprintf(&Hex_Num[0],"0x%04X",(const uint16_t)((_val >> 32) & 0xFFFF));
		sprintf(&Hex_Num[6],"%04X",(const uint16_t)((_val >> 16) & 0xFFFF));
		sprintf(&Hex_Num[10],"%04X\n\r",(const uint16_t)((_val >> 0) & 0xFFFF));
		usart_serial_write_packet(USART_SERIAL,(const uint8_t*)Hex_Num,strlen(Hex_Num));
		//processa_resultado(val);
	}
}

void FSK_Decoding(void)
{
	static uint8_t short_periods = 0;
	static uint8_t long_periods = 0;
	
	uint16_t duracao = tc45_read_count(&TCC5);
	
	//if (duracao < GLITCH_TIME)
	//	return;

	// Reset counter
	Reinicia_Contagem_TC5();
	
	if (TCC5.INTFLAGS & (1<<0))
	{
		duracao = 65535;
		TCC5.INTFLAGS = (1<<0);
	}
	
	if(duracao >= FSK_SHORT_PERIOD_MIN && duracao <= FSK_SHORT_PERIOD_MAX)
	{
		if (long_periods != 0)
		{
			// Ocorreu uma transicao. Estavamos recebendo pulsinhos longos,
			// e agora recebeu pulsinho curto.
			if (long_periods >= 4 && long_periods <= 7)
			{
				// Pulso curto, formado de pulsinhos longos (correspondente, portanto, a nivel 1)
				ioport_set_pin_level(D1_DATA,0);
				leu_bit(1);
			}
			else if (long_periods >= 8 && long_periods <= 14)
			{
				// Pulso longo, formado de pulsinhos longos (correspondente, portanto, a nivel 1)
				ioport_set_pin_level(D1_DATA,0);
				leu_bit(1);
				leu_bit(1);
			}
			
			else if (long_periods > 14)
			{
				// Encontramos a sequencia de bit que sincronizam o cartao
				comeca_leitura();
			}
			else
			{
				// Erro!
				delay_us(10);
				ioport_toggle_pin(D1_DATA);
				delay_us(10);
				ioport_toggle_pin(D1_DATA);
				aborta_leitura();
			}
		}
		long_periods = 0;
		short_periods++;
	}
	else if(duracao >= FSK_LONG_PERIOD_MIN && duracao <= FSK_LONG_PERIOD_MAX)
	{
		if (short_periods != 0)
		{
			// Ocorreu uma transicao. Estavamos recebendo pulsinhos curtos,
			// e agora recebeu pulsinho longo.
			if (short_periods >= 4 && short_periods <= 7)
			{
				// Pulso curto, formado de pulsinhos pequenos (correspondente, portanto, a nivel 0)
				ioport_set_pin_level(D1_DATA,1);
				leu_bit(0);
			}
			else if (short_periods >= 8 && short_periods <= 14)
			{
				// Pulso longo, formado de pulsinhos pequenos (correspondente, portanto, a nivel 0)
				ioport_set_pin_level(D1_DATA,1);
				leu_bit(0);
				leu_bit(0);
			}
			else
			{
				// Erro!
				delay_us(10);
				ioport_toggle_pin(D1_DATA);
				delay_us(10);
				ioport_toggle_pin(D1_DATA);
				//TODO: Recomecar leitura
			}
		}
		short_periods = 0;
		long_periods++;
	}
	else 
	{
		long_periods = 0;
		short_periods = 0;
	}
}