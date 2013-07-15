/*
 * PSK_Decode.c
 *
 * Created: 12/07/2013 14:44:07
 *  Author: Neto
 */ 

#include <asf.h>
#include "FSK_Decode.h"

uint8_t  _valendo=0;

static inline void leu_bit(uint8_t bit)
{
	if(!valendo)
	{
		if(bit)
		{
			
		}
}

void FSK_Decoding(void)
{
	uint8_t short_period=0;
	uint8_t long_period=0;
	
	duracao = tc45_read_count(&TCC5);
	
	if (duracao < GLITCH_TIME)
		return;

	// Reset counter
	Reinicia_Contagem_TC5();
	
	if (TCC5.INTFLAGS & (1<<0))
	{
		duracao = 65535;
		TCC5.INTFLAGS = (1<<0);
	}
	
	if(duracao >= FSK_LOW_PERIOD_MIN && duracao <= FSK_LOW_PERIOD_MAX)
	{
		if(++short_period == 5)
		{
			short_period=0;
			leu_bit(0);
		}
	}
	else if(duracao >= FSK_HIGH_PERIOD_MIN && dduracao <= FSK_HIGH_PERIOD_MAX)
	{
		if(++long_period == 6)
		{
			long_period=0;
			leu_bit(1);
		}
	}
	else 
	{
		long_period=0;
		short_period=0;
	}
}