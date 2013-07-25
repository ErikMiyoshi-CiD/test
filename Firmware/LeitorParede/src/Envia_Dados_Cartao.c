/*
* Envia_Dados_Cartao.c
*
* Created: 25/07/2013 09:46:41
*  Author: ENGENHARIA2
*/

#include <asf.h>
#include "config_board.h"
#include "Envia_Dados_Cartao.h"
#include "ABATK2.h"
#include "SerialOut.h"
#include "Wiegand.h"

static uint32_t _ultimo_cartao=0;

static uint16_t _tempo_agora=0;

static uint8_t _repeticoes=0;

void Enviar_Dados_Cartao(uint32_t card_data)
{	
	_tempo_agora=Cont_Timer_XCL_16bits();
	
	if((XCL.INTFLAGS && (1<<6))==1)
	{
		xcl_tc16_clear_underflow_interrupt();
		Reinicia_XCL_Cnt();
		_repeticoes=1;
	}
	
	if(_ultimo_cartao!=card_data)
	{
		// Novo cartao
		_ultimo_cartao = card_data;
		_repeticoes=1;
		Reinicia_XCL_Cnt();
	}
	else
	{	
		if (_tempo_agora < REPEAT_TIMEOUT)
		{
			_repeticoes=1;
		}
		Reinicia_XCL_Cnt();
		
		if(_repeticoes < MIN_REPEAT)
		{
			_repeticoes++;
		}
		if(_repeticoes == MIN_REPEAT)
		{
			_repeticoes++;
			Transmite_Cartao_Serial(card_data);
			//Transmite_Pacote_Wiegand(card_data);
			//Enviar_ABA_TK2(card_data);
		}
	}
}