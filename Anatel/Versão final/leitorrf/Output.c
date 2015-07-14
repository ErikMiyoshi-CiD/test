/*
 * Output.c
 *
 * Created: 2014-03-05 8:34:18 PM
 *  Author: Alb
 */ 

#include "sam.h"
#include "Output.h"
#include "Helper.h"
#include "delay.h"

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
	delay_ms(250);
	led_red();
	delay_ms(500);
	led_idle();
}


///Envia os dados pela saída selecionada
void go_output(uint32_t data)
{
	go_output_internal(data);
}