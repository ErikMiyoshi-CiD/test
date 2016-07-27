#include <asf.h>

#include "Output.h"
#include "Helper.h"
#include "ABATK2.h"
#include "Wiegand.h"
#include "SerialOut.h"

static struct tc_module tc_timeout;

//Esta função inicializa o clock de OUTPUT_CARD_TIMEOUT ms
void timeout_timer_init(void){
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	config_tc.oneshot = true;
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_16_bit.compare_capture_channel[0] = 48000000/1024*OUTPUT_CARD_TIMEOUT_MS/1000 - 1;
	tc_init(&tc_timeout, TC2, &config_tc);
}

void timeout_timer_start(void)
{
	TC2->COUNT16.COUNT.reg = 0;
	TC2->COUNT16.CC[0].reg=48000000/1024*OUTPUT_CARD_TIMEOUT_MS/1000 - 1;
	tc_enable(&tc_timeout);
}

void timeout_timer_stop(void)
{
	tc_disable(&tc_timeout);
}

static void go_output_internal(uint32_t data)
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
	led_idle();
}


///Envia os dados pela saída selecionada
void go_output(uint32_t data)
{
	static int is_initialized=0;
	static int total_timeout_ms;
	int count;
	
	if (is_initialized==0)
	{
		is_initialized=1;
		timeout_timer_init();
		timeout_timer_start();
		
		//Da primeira vez vamos cuspir os dados
		go_output_internal(data);
		total_timeout_ms=0;
	}
	else
	{
		count = TC2->COUNT16.COUNT.reg;
		if (count == 0)
		{
			//Passou timeout
			go_output_internal(data);
			total_timeout_ms=0;
		}
		else
		{
			total_timeout_ms+=1024*count/48000;
			if (total_timeout_ms>=OUTPUT_CARD_FORCE_OUTPUT_TIMEOUT_MS) {
				go_output_internal(data);
				total_timeout_ms=0;
			}
		}
		timeout_timer_stop();
		timeout_timer_start();
		
		DEBUG_PUTSTRING(".");
	}
}