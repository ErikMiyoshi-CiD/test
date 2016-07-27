#include <asf.h>
#include "SerialOut.h"
#include "pinos.h"

void bitbang_putchar(uint8_t c)
{
	const int us_tbit = 48000000/9600;
	int i;

	ioport_set_pin_level(PIN_D0_TX_CLK,1);
	delay_cycles(us_tbit);
	for (i=0;i<8;i++)
	{
		if ((c & (1 << i))!=0)
		ioport_set_pin_level(PIN_D0_TX_CLK,0);
		else
		ioport_set_pin_level(PIN_D0_TX_CLK,1);
		
		delay_cycles(us_tbit);
	}
	ioport_set_pin_level(PIN_D0_TX_CLK,0);
	delay_cycles(us_tbit);
}

void bitbang_putstring(const char* string, int size)
{
	int i;

	for (i=0;i<size;i++)
		bitbang_putchar((uint8_t)string[i]);
}

void configure_usart(void)
{
	DEBUG_PUTSTRING("initializing USART\n\r");
	ioport_set_pin_dir(PIN_D0_TX_CLK, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(PIN_D0_TX_CLK,0);
	DEBUG_PUTSTRING("USART Initialised\n\r");
}


