/*
 * PSK_Decode.c
 *
 * Created: 17/07/2013 17:58:39
 *  Author: Neto
 */ 

#include <asf.h>
#include "config_board.h"
#include "PSK_Decode.h"

static int8_t _primeira_vez = 0;

void PSK_Decoding(void)
{
	static uint16_t mov_avg_s0=0;
	static uint16_t mov_avg_s1=0;
	static uint16_t mov_avg_s2=0;
	static uint16_t mov_avg_s3=0;
	
	static uint8_t holdoff=0;
	
	static uint16_t last_time;
	
	uint16_t media=0;
	uint16_t amostra=0;
	uint16_t elapsed;
	
	amostra = tc45_read_count(&TCC5);
	
	if (_primeira_vez)
	{
		last_time = amostra;
		return;
	}
	
	elapsed = amostra - last_time;
	last_time = amostra;
	
	ioport_toggle_pin(USART_TX_PIN);
	
	if (elapsed > 8191)
		elapsed = 8191;
	
	mov_avg_s3=mov_avg_s2;
	mov_avg_s2=mov_avg_s1;
	mov_avg_s1=mov_avg_s0;
	mov_avg_s0=elapsed;
	
	media=(mov_avg_s0+mov_avg_s1+mov_avg_s2+mov_avg_s3) / 4;
	
	ioport_toggle_pin(USART_TX_PIN);
	
	if(holdoff==0)
	{
		if (media >= EXPECTED_TRANS_AVG_MIN_TIME && media <= EXPECTED_TRANS_AVG_MAX_TIME)
		{
			ioport_toggle_pin(CARD_PRES);
			holdoff++;
		}
	}
	else if (holdoff <= 4)
	{
		holdoff++;
	}
	else
	{
		holdoff=0;
	}
}