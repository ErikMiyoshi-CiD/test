/*
 * PSK_Decode.c
 *
 * Created: 17/07/2013 17:58:39
 *  Author: ENGENHARIA2
 */ 

#include <asf.h>
#include "config_board.h"
#include "PSK_Decode.h"

//#define SIZE_BUF_PSK 8

//static uint16_t delta_phys[SIZE_BUF_PSK];
//static uint8_t circ_buf_pos = SIZE_BUF_PSK - 1; //Primeira inserção é em 0
static int8_t _primeira_vez = 0;
//
//static void inline insert_circ_buf(uint16_t delta_phy)
//{
	//circ_buf_pos++;
	//if (circ_buf_pos >= SIZE_BUF_PSK)
		//circ_buf_pos=0;
	//
	//delta_phys[circ_buf_pos]=delta_phy;
//}
//
//static uint16_t inline erro_circ_buf (void)
//{
	//uint8_t i;
	//uint16_t media=0;
	//uint16_t erro=0;
	//
	//for (i=0;i<SIZE_BUF_PSK;i++)
	//{
		//media += delta_phys[i];
	//}
	//media=media/SIZE_BUF_PSK;
	//
	//for (i=0;i<SIZE_BUF_PSK;i++)
	//{
		//if (delta_phys[i]>=media)
			//erro+=delta_phys[i]-media;
		//else
			//erro+=media-delta_phys[i];
	//}	
	//
	//if (media==0)
		//return 0xFFFF;
	//else
		//return erro;
//}

//void PSK_Decoding(void)
//{
	//static uint16_t t_lastcount;
	//static uint8_t synched = 0;
	//uint16_t t_atual;
	//
	//ioport_toggle_pin(USART_TX_PIN);	
//
	//t_atual=tc45_read_count(&TCC5);
	//Reinicia_Contagem_TC5();	
	//
	//if (_primeiro==0)
	//{
		//_primeiro=1;
		//goto end;
	//}
	//insert_circ_buf(t_atual - t_lastcount);
	//
	////Vamos ver se estamos sincronizados
	//if (erro_circ_buf() < 80)
	//{
		//synched = 1;
	//}
	//else
	//{
		//synched = 0;
	//}
//
//end:
	//ioport_set_pin_level(CARD_PRES,!synched);
	//t_lastcount=t_atual;
	//ioport_toggle_pin(USART_TX_PIN);
	//return;
//}

void PSK_Decoding_2(void)
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
		#define EXPECTED_TRANS_AVG_MAX_TIME 480
		#define EXPECTED_TRANS_AVG_MIN_TIME 450
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