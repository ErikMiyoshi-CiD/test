/*
* PSK_Decode.c
*
* Created: 17/07/2013 17:58:39
*  Author: Neto
*/

#include <asf.h>
#include "stdio.h"
#include "string.h"
#include "config_board.h"
#include "PSK_Decode.h"
#include "Envia_Dados_Cartao.h"
#include "Wiegand.h"

static int8_t _primeira_vez = 0;
static uint8_t _num_bits=0;
static uint8_t _sync=0;
static uint64_t _val;


static inline void Recebe_Bit(uint8_t bit);
static inline void processa_resultado(uint64_t val);

static inline void reinicia_leitura(void)
{
	_val = 0;
	_num_bits = 0;
}

void PSK_Decoding(void)
{
	static uint16_t mov_avg_s0=0;
	static uint16_t mov_avg_s1=0;
	static uint16_t mov_avg_s2=0;
	static uint16_t mov_avg_s3=0;
	
	static uint8_t holdoff=0;
	
	static uint8_t bit_status=1;
	
	static uint16_t last_time;
	
	static uint16_t pulse_count=0;
	
	static uint8_t i=0;
	
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
	
	pulse_count++;
	
	if (elapsed > 8191)
	{
		elapsed = 8191;
	}
	
	mov_avg_s3=mov_avg_s2;
	mov_avg_s2=mov_avg_s1;
	mov_avg_s1=mov_avg_s0;
	mov_avg_s0=elapsed;
	
	media=(mov_avg_s0+mov_avg_s1+mov_avg_s2+mov_avg_s3) / 4;
	
	if(holdoff==0)
	{
		if ( (media >= EXPECTED_TRANS_AVG_MIN_TIME && media <= EXPECTED_TRANS_AVG_MAX_TIME) )
		{
			if(_sync==1)
			{
				if (pulse_count >= 8)
				{
					pulse_count += 8;
					for(i=(pulse_count>>4) - 1; i>0; i--)
					{
						Recebe_Bit(bit_status);
					}
					pulse_count = pulse_count & 0xf;
					if (pulse_count < 5 || pulse_count > 11)
					{
						// Erro!
						_sync = 0;
						pulse_count = 0;
						return;
					}
					bit_status=!bit_status;
				}
				else
				{
					// Pulso muito curto! Erro!
					_sync = 0;
					pulse_count = 0;
					return;
				}
				Recebe_Bit(bit_status);
			}
			else if(pulse_count > 400)
			{
				_sync=1;
				reinicia_leitura();
				bit_status=1;
				Recebe_Bit(bit_status);
			}
			
			pulse_count=0;
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

static inline void Recebe_Bit(uint8_t bit)
{
	_val = _val << 1;
	_val |= bit;
	if (++_num_bits >= 35)
	{
		_sync=0;
		processa_resultado(_val);
	}
}

static inline void processa_resultado(uint64_t val)
{	
	uint16_t card_number;
	
	uint8_t site_code;
	
	uint32_t card_data;
	
	if((val & 0x5)==0) 
	{
		// Erro!! Fim do cartão está errado
		reinicia_leitura(); 
		return;
	}
	
	// S ??A0 1AAA72BBBB54CCCCD36DDD?PP?? TTT
	// 4 3210 987654321098765432109876543 210
	//	    3		   2		 1
	
	site_code = 
	(((val >> 30) & 0x1) << 7) |	// Bit 0 - MSB
	(((val >> 29) & 0x1) << 6) |	// Bit 1
	(((val >> 24) & 0x1) << 5) |	// Bit 2
	(((val >> 12) & 0x1) << 4) |	// Bit 3
	(((val >> 18) & 0x1) << 3) |	// Bit 4
	(((val >> 19) & 0x1) << 2) |	// Bit 5
	(((val >> 11) & 0x1) << 1) |	// Bit 6
	((val >> 25) & 0x1);			// Bit 7 - LSB
	
	card_number = 
	((val >> 8) & 0x7u) |
	(((val >> 13) & 0x1Fu) << 3) |
	(((val >> 20) & 0xFu) << 8)  |
	(((val >> 26) & 0x7u) << 12) |
	(((val >> 31) & 0x1u) << 15);
	
	card_data = ( ((uint32_t) site_code << 16) | card_number );

	Enviar_Dados_Cartao(card_data);
}