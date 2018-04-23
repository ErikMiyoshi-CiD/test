#include "stm32f0xx_hal.h"
#include "tim.h"

#include "delay.h"

void delay_init() 
{
		MX_TIM1_Init();
}

void delay_cycles(uint16_t cycles) 
{
	if (cycles > 154)	// Periodo minimo que pode ser contado 
	{
		__HAL_TIM_SET_COUNTER(&htim1, 0);
		HAL_TIM_Base_Start(&htim1);
		
		while (__HAL_TIM_GET_COUNTER(&htim1) < cycles - 730);
		
		HAL_TIM_Base_Stop(&htim1);
	}
}

void delay_us(uint16_t us) 
{
	uint16_t ms;
	uint16_t us_new;
	
	ms = us / 1000;
	us_new = us % 1000;
	
	if (us > 3)	{
		while (ms > 0) {
			delay_cycles(48 * 1000);
			
			ms--;
		}
		
		delay_cycles(48 * us_new);
	}
}
