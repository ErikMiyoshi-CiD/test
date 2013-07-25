/*
 * FSK_Decode.c
 *
 * Created: 12/07/2013 14:44:07
 *  Author: Neto
 */ 

#include <asf.h>
#include "Envia_Dados_Cartao.h"
#include "FSK_Decode.h"
#include "SerialOut.h"
#include "Wiegand.h"
#include "ABATK2.h"
#include "config_board.h"
#include "string.h"
#include <stdio.h>

static uint64_t _val;
static int _last_bit;
static int _num_bits;
static int _valendo;

static inline void processa_resultado(void);

static void comeca_leitura(void)
{
	_val = 0;
	_num_bits = 0;
	_valendo = 1;
}

static void aborta_leitura(void)
{
	_valendo = 0;
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
			_val |= 0;
		}
		else
		{
			aborta_leitura();
			return;
		} 

	}
	_last_bit = bit;
	++_num_bits;

	if (_num_bits >= 90)
	{
		// Acabou!
		aborta_leitura();
		processa_resultado();
	}
}

static inline void processa_resultado()
{
	uint32_t card_data;

	card_data = 
	((_val >> 1) & 0xFFFF) |
	(((_val >> 17) & 0xFF) << 16);
	
	Enviar_Dados_Cartao(card_data);
}

void FSK_Decoding(void)
{
	static uint8_t short_periods = 0;
	static uint8_t long_periods = 0;
	
	uint16_t duracao = tc45_read_count(&TCC5);

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
				leu_bit(1);
			}
			else if (long_periods >= 8 && long_periods <= 14)
			{
				// Pulso longo, formado de pulsinhos longos (correspondente, portanto, a nivel 1)
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
				leu_bit(0);
			}
			else if (short_periods >= 8 && short_periods <= 14)
			{
				// Pulso longo, formado de pulsinhos pequenos (correspondente, portanto, a nivel 0)
				leu_bit(0);
				leu_bit(0);
			}
			else if (short_periods > 14)
			{
				leu_bit(0);
			}
			else
			{
				// Erro!
				aborta_leitura();
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