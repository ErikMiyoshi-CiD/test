#include "stm32f0xx_hal.h"

#include "main.h"
#include "helper.h"
#include "Output.h"
#include "RF_Mifare.h"
#include "RF_Mifare_Util.h"
#include "RC522_Mifare.h"
#include "RC522_ErrCode.h"

#include "i2c.h"
#include "iwdg.h"

void mifare_i2c_init(void)
{
	MX_I2C1_Init();
}

void mifare_reset(void)
{
	HAL_Delay(200);
	HAL_GPIO_WritePin(ASK_IN_GPIO_Port, ASK_IN_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(ASK_IN_GPIO_Port, ASK_IN_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
}

void mifare_activate_card(void)
{
	unsigned char bSAK, baATQ[2], uid[8], uid_length=255 /*nunca vai ser*/;
	uint64_t uid32;
	uint64_t uid64;

	Rc522RFReset(5);

	if (ActivateCard(ISO14443_3_REQA, baATQ, uid, &uid_length, &bSAK) == STATUS_SUCCESS)
	{
		if (uid_length > 4)
		{
			uid64=((uint64_t)uid[0] << 56) + ((uint64_t)uid[1] << 48) + ((uint64_t)uid[2] << 40) + ((uint64_t)uid[3] << 32) +
					((uint64_t)uid[4] << 24) + ((uint64_t)uid[5] << 16) + ((uint64_t)uid[6] << 8) + (uint64_t)uid[7]; //MSByte (uid0 é o mais significativo)
			go_output(uid64,wiegand_size);			
		}
		else
		{
			uid32=((uint32_t)uid[0] << 24) + ((uint32_t)uid[1] << 16) + ((uint32_t)uid[2] << 8) + (uint32_t)uid[3]; //MSByte (uid0 é o mais significativo)
			go_output(uid32,wiegand_size);
		}
	}
}

void Mifare_Init(void) 
{
	mifare_i2c_init();
	
	/* Reconfigure the ASK_IN pin for Mifare. 
	 * This is ok because we read B0/B1/B2 and we know that we have a REPMIF installed
	 */
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = ASK_IN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ASK_IN_GPIO_Port, &GPIO_InitStruct);
	
	mifare_reset();
		
	Rc522Init();
}

void Mifare_Run(void)
{
	while (1)
	{
		// '0' em LED_INPUT liga o led verde e o buzzer
		if (HAL_GPIO_ReadPin(LED_INPUT_GPIO_Port, LED_INPUT_Pin) == GPIO_PIN_RESET) {
			led_green();
			buzz_on();
		} else {
			led_idle();
			buzz_off();
		}
		
		mifare_activate_card();
		HAL_IWDG_Refresh(&hiwdg);
	}
}
