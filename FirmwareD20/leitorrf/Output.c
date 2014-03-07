/*
 * Output.c
 *
 * Created: 2014-03-05 8:34:18 PM
 *  Author: Alb
 */ 

#include "sam.h"
#include "Output.h"
#include "Helper.h"
#include "ABATK2.h"
#include "Wiegand.h"

//Esta função inicializa o clock de OUTPUT_CARD_TIMEOUT ms
void timeout_timer_init(void){
	//Habilita o clock do TC2
	PM->APBCMASK.reg |= PM_APBCMASK_TC2;
	
	//Reseta o TC2
	TC2->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC2->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC2
	TC2->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, não roda em STBY, MFRQ, COUNT16, Disable
	TC2->COUNT16.CTRLA.bit.PRESCALER=0x7;
	TC2->COUNT16.CTRLBSET.reg = TC_CTRLBSET_ONESHOT; //Count up uma vez
	TC2->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída
	TC2->COUNT16.CC[0].reg = (uint16_t)OUTPUT_CARD_TIMEOUT_CYCLES; //Vamos contar
	
	//Habilita o TC2
	//TC2->COUNT16.CTRLA.bit.ENABLE = 1;
}

volatile uint16_t tango;
void timeout_timer_start(void)
{
	TC2->COUNT16.CC[0].reg = (uint16_t)OUTPUT_CARD_TIMEOUT_CYCLES; //Vamos contar
	TC2->COUNT16.COUNT.reg = 0;
	TC2->COUNT16.CTRLA.bit.ENABLE=1;	
}

void timeout_timer_stop(void)
{
	TC2->COUNT16.CTRLA.bit.ENABLE=0;
}

void go_output_internal(uint32_t data)
{
	led_green();
	switch (tipo_output)
	{
		case OUTPUT_ABATRACK:
			Enviar_ABA_TK2(data);
			break;
		case OUTPUT_WIEGAND:
			Transmite_Pacote_Wiegand(data);
			break;		
	}
	buzz(50); //Agora sim busy wait 100ms
	led_yellow();
}


///Envia os dados pela saída selecionada
void go_output(uint32_t data)
{
	static int is_initialized=0;
	
	if (is_initialized==0)
	{
		is_initialized=1;
		timeout_timer_init();
		timeout_timer_start();
		
		//Da primeira vez vamos cuspir os dados
		go_output_internal(data);
	}
	else
	{
		if (TC2->COUNT16.COUNT.reg == 0)
		{
			//Passo ou timeout, é um cartão novo! Cospe tudo
			go_output_internal(data);
			
		}
		timeout_timer_stop();
		timeout_timer_start();
	}
}