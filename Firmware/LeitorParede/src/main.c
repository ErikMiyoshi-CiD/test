#include <asf.h>
#include "config_board.h"

#include "ASK_Decode.h"
#include "FSK_Decode.h"
#include "PSK_Decode.h"

int main (void)
{
	Inicializa_Placa();

	Inicializa_GPIO();

	Inicializa_USART();

	Liga_125kHz();

	Configura_TC5();

	TC5_Set_CountUp_Mode();

	Liga_TC5();
	
	Conf_Timer_XCL_16bits();
	
	Liga_Timer_XCL_16bits();

	#ifdef ASK
	ASK_Pin_Config();
	usart_putchar(USART_SERIAL,'A');
	usart_putchar(USART_SERIAL,'S');
	usart_putchar(USART_SERIAL,'K');
	#endif

	#ifdef FSK
	FSK_Pin_Config();
	usart_putchar(USART_SERIAL,'F');
	usart_putchar(USART_SERIAL,'S');
	usart_putchar(USART_SERIAL,'K');
	#endif

	#ifdef PSK
	PSK_Pin_Config();
	usart_putchar(USART_SERIAL,'P');
	usart_putchar(USART_SERIAL,'S');
	usart_putchar(USART_SERIAL,'K');
	#endif
	
	while(1)
	{
	}
}

#ifndef	PSK
	ISR(PORTD_INT_vect)
	{
		#ifdef ASK
			ASK_Decoding();
		#endif
		
		#ifdef FSK
			FSK_Decoding();
		#endif
		
		Clear_PORTD_Int_Flag();
	}
#endif

#ifdef PSK
	ISR(PORTC_INT_vect)
	{
		PSK_Decoding();
		Clear_PORTC_Int_Flag();
	}
#endif