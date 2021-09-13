#include "stm32f0xx_hal.h"

#include "Output.h"
#include "helper.h"
#include "ABATK2.h"
#include "Wiegand.h"
#include "SerialOut.h"

#include "tim.h"

//Esta função inicializa o clock de OUTPUT_CARD_TIMEOUT ms
void timeout_timer_init(void){
	MX_TIM17_Init();
}

void timeout_timer_start(void)
{
	__HAL_TIM_SET_COUNTER(&htim17, 0);
	
	HAL_TIM_Base_Start_IT(&htim17);
}

void timeout_timer_stop(void)
{
	HAL_TIM_Base_Stop_IT(&htim17);
}

static void go_output_internal(uint64_t data, uint8_t size)
{	
	led_green();
	switch (tipo_output)
	{
		case OUTPUT_ABATRACK:
			Enviar_ABA_TK2(data);
		break;
		
		case OUTPUT_WIEGAND:
			TxWiegandPacket(data,size);
		break;	

		case OUTPUT_RS232:
			Enviar_RS232(data);
		break;			
	}
	buzz(50); //Agora sim busy wait 100ms
	led_idle();
}


///Envia os dados pela saída selecionada
void go_output(uint64_t data, uint8_t size)
{
	static int is_initialized = 0;
	static int total_timeout_ms;
	int count;
	
	if (is_initialized == 0)
	{
		is_initialized = 1;
		timeout_timer_init();
		timeout_timer_start();
		
		//Da primeira vez vamos cuspir os dados
		go_output_internal(data, size);
		total_timeout_ms = 0;
	}
	else
	{
		count = __HAL_TIM_GET_COUNTER(&htim17);
		if (count == 0)
		{
			//Passou timeout
			go_output_internal(data,size);
			total_timeout_ms = 0;
		}
		else
		{
			total_timeout_ms += 1024 * count / 48000;
			if (total_timeout_ms >= OUTPUT_CARD_FORCE_OUTPUT_TIMEOUT_MS) {
				go_output_internal(data, size);
				total_timeout_ms = 0;
			}
		}
		timeout_timer_stop();
		timeout_timer_start();
		
		DEBUG_PUTSTRING(".");
	}
}
