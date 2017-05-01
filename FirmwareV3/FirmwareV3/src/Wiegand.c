#include <asf.h>

#include "helper.h"
#include "pinos.h"
#include "delay.h"

#include "Wiegand.h"

#define WIE_NUMDIGITS		24  	// Número de bits de dados no frame Wiegand a ser transmitido
#define DATA_PULSE_TIME		100		// Intervalo de tempo em que o pulso de dados é mantido em nível baixo - Unidade: microsegundos
#define DATA_INTERVAL_TIME	2000	// Intervalo de tempo entre os pulsos de dados - Unidade: milisegundos

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

static inline uint64_t WiegandEncode(uint64_t val, uint8_t size)
{
	uint64_t LeadingParity;
	uint64_t TrailingParity;
	
	switch (size)
	{
		case 26:	
			//Garante que valor tem no máximo 24 bits
			val = val & 0xFFFFFF;
			
			if (CalculateParityOdd( (val>>12) & 0xFFF ))
				LeadingParity=0;
			else
				LeadingParity=1;
			
			if (CalculateParityOdd( val & 0xFFF ))
				TrailingParity=1;
			else
				TrailingParity=0;		
	
			return ( (LeadingParity << (size-1)) | (val << 1) | TrailingParity );
			break;
		case 34:
			//Garante que valor tem no máximo 32 bits
			val = val & 0xFFFFFFFF;
			
			if (CalculateParityOdd( (val>>16) & 0xFFFF ))
				LeadingParity=0;
			else
				LeadingParity=1;
						
			if (CalculateParityOdd( val & 0xFFFF ))
				TrailingParity=1;
			else
				TrailingParity=0;
						
			return ( (LeadingParity << (size-1)) | (val << 1) | TrailingParity );
			
			break;
		default:
			return val;
	}

}

static inline void WiegandSendPayload(uint64_t mensagem, uint8_t size)
{
	uint8_t tmp;
	int8_t i;
	
	for(i=size-1; i>=0;i--)
	{
		tmp = !!(mensagem & (((uint64_t)1)<<i));
		
		if(tmp)
		{
			ioport_set_pin_level(PIN_D1_DATA,1); // Transitor de saida inverte o sinal
			delay_us(DATA_PULSE_TIME);
			ioport_set_pin_level(PIN_D1_DATA,0);
		}
		else
		{
			ioport_set_pin_level(PIN_D0_TX_CLK,1);
			delay_us(DATA_PULSE_TIME);
			ioport_set_pin_level(PIN_D0_TX_CLK,0);
		}
		
		delay_us(DATA_INTERVAL_TIME);
	}
}

void TxWiegandPacket(uint64_t card_num, uint8_t size)
{
	uint64_t transmit_message;
	
	transmit_message = WiegandEncode(card_num,size);
	WiegandSendPayload(transmit_message,size);
}