#include "stm32f0xx_hal.h"
#include "SerialOut.h"
#include "main.h"
#include "tim.h"

#include "delay.h"

#include "helper.h"
#include "RF_common.h"

#define NASK	10
#define NMIF	8

static uint8_t _rs232_buff[NASK];

void bitbang_putchar(uint8_t c)
{
	const int us_tbit = 1000000/9600;
	int i;

	__disable_irq();
	
	HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_SET); //Start bit = 0 (this pin is inverted, so set)
	delay_us(us_tbit);
	
	for (i = 0; i < 8; i++) 
	{
		if ((c & (1 << i)) != 0)
			HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_RESET); //If bit is set, set output to high (this pin is inverted, so reset)
		else
			HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_SET); //If bit is 0, set output to low (this pin is inverted, so set)
		
		delay_us(us_tbit);
	}
	
	HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_RESET); //STOP = IDLE = 1 (this pin is inverted)
	delay_us(us_tbit);
	
	__enable_irq();
}

void bitbang_putstring(const char* string, int size)
{
	int i;

	for (i = 0; i < size; i++)
		bitbang_putchar((uint8_t) string[i]);
}

void Enviar_RS232(uint64_t data)
{
	uint8_t i, nBytes;
	uint64_t auxData;
	
	switch (tipo_leitor) {
		case TIPO_ASK:			// <site> <id>
			nBytes = NASK;
		
			auxData = data;
			for (i = 0; i < NASK; i++) {
				_rs232_buff[NASK - 1 - i] = (auxData % 10) + '0';
				auxData /= 10;
			}
		break;
		
		case TIPO_MIFARE: 	// [MSB] ... [LSB]
			nBytes = NMIF;
		
			for (i = 0; i < NMIF; i++) {
				_rs232_buff[NMIF-1-i] = dec2hex((data >> 4*i) & 0xF);
			}
		break;
		
		default:
		break;
	}
	
	bitbang_putchar(0x02); // STX - Start of text
	
	for (i = 0; i < nBytes; i++) {
		bitbang_putchar(_rs232_buff[i]);
	}	
	
	bitbang_putchar(0x0D); // CR - Carriage return
	bitbang_putchar(0x0A); // LF - Line feed
	bitbang_putchar(0x03); // ETX - End of text
}

uint8_t dec2hex(uint8_t val) {
    if (val <= 9) {
        return val + '0';
    } else if (val >= 10 && val <= 15) {
        return (val - 10) + 'A';
    } else {
        return 0;
    }
}

void configure_usart(void)
{
	DEBUG_PUTSTRING("initializing USART\n\r");
	HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_SET);
	DEBUG_PUTSTRING("USART Initialised\n\r");
}
