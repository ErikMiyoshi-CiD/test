#include "stm32f0xx_hal.h"

#include "helper.h"
#include "main.h"
#include "delay.h"

#include "Wiegand.h"

#define DATA_PULSE_TIME		50		// Intervalo de tempo em que o pulso de dados � mantido em n�vel baixo - Unidade: microsegundos
#define DATA_INTERVAL_TIME	2000	// Intervalo de tempo entre os pulsos de dados - Unidade: microsegundos

static inline uint32_t CalculateParityOdd(uint32_t val) 
{
	uint32_t parity = 0;
	
	while (val)
	{
		parity ^= val & 0x1;
		val >>= 1;
	}
	
	return !parity; // Paridade impar
}

static inline WiegandFrame WiegandEncode(uint64_t val, uint8_t size)
{
	WiegandFrame wf;
	
	//Inicializa��o do Wiegand frame
	wf.leading_parity = 0;
	wf.trailing_parity = 0;
	wf.data = 0;
	
	switch (size)
	{
		case 26:	
			//Garante que valor tem no m�ximo 24 bits
			val = val & 0xFFFFFF;
			
			if (CalculateParityOdd( (val>>12) & 0xFFF ))
				wf.leading_parity = 0;
			else
				wf.leading_parity = 1;
			
			if (CalculateParityOdd( val & 0xFFF ))
				wf.trailing_parity = 1;
			else
				wf.trailing_parity = 0;
	
			wf.data = val;
			
			return wf;
			
			break;

		case 32:
			//Garante que valor tem no m�ximo 32 bits
			val = val & 0xFFFFFFFF;

			wf.data = val;

			return wf;
			
			break;

		case 34:
			//Garante que valor tem no m�ximo 32 bits
			val = val & 0xFFFFFFFF;
			
			if (CalculateParityOdd( (val>>16) & 0xFFFF ))
				wf.leading_parity = 0;
			else
				wf.leading_parity = 1;
						
			if (CalculateParityOdd( val & 0xFFFF ))
				wf.trailing_parity = 1;
			else
				wf.trailing_parity = 0;
						
			wf.data = val;
			
			return wf;
			
			break;
		case 66:
			if (CalculateParityOdd( (val>>32) & 0xFFFFFFFF ))
				wf.leading_parity = 0;
			else
				wf.leading_parity = 1;
					
			if (CalculateParityOdd( val & 0xFFFFFFFF ))
				wf.trailing_parity = 1;
			else
				wf.trailing_parity = 0;
					
			wf.data = val;
					
			return wf;
					
			break;
		default:
			return wf;
	}

}

static inline void WiegandSend1(void)
{
	HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_SET); // Transistor de saida inverte o sinal
	delay_us(DATA_PULSE_TIME);
	HAL_GPIO_WritePin(D1_TX_CLK_GPIO_Port, D1_TX_CLK_Pin, GPIO_PIN_RESET);
	delay_us(DATA_INTERVAL_TIME);
}

static inline void WiegandSend0(void)
{
	HAL_GPIO_WritePin(D0_DATA_GPIO_Port, D0_DATA_Pin, GPIO_PIN_SET); // Transistor de saida inverte o sinal
	delay_us(DATA_PULSE_TIME);
	HAL_GPIO_WritePin(D0_DATA_GPIO_Port, D0_DATA_Pin, GPIO_PIN_RESET);
	delay_us(DATA_INTERVAL_TIME);
}

static inline void WiegandSendPayload(WiegandFrame wf, uint8_t size)
{
	uint8_t tmp;
	int8_t i;

	/** Ignore parities for W32. */
	if (WIEGAND_32 == size)
	{
		for(i = size - 1; i >= 0; i--) //-3 em raz�o do 66 que deve ser 64. 66 --> De 63 a 0
		{
		tmp = !!(wf.data & (((uint64_t) 1) << i));
		
		if(tmp)
			WiegandSend1();
		else
			WiegandSend0();	
		}
	}
	else
	{
		if (wf.leading_parity)
			WiegandSend1();
		else
			WiegandSend0();
		
		for(i = size - 3; i >= 0; i--) //-3 em raz�o do 66 que deve ser 64. 66 --> De 63 a 0
		{
			tmp = !!(wf.data & (((uint64_t) 1) << i));
			
			if(tmp)
				WiegandSend1();
			else
				WiegandSend0();	
		}
		
		if (wf.trailing_parity)
			WiegandSend1();
		else
			WiegandSend0();
	}
}

void TxWiegandPacket(uint64_t card_num, uint8_t size)
{
	WiegandFrame wf;
	
	wf = WiegandEncode(card_num, size);
	WiegandSendPayload(wf, size);
}