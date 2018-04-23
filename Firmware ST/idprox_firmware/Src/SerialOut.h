#ifndef SERIAL_H_
#define SERIAL_H_

#define SERIAL_DEBUG 0

#if SERIAL_DEBUG
	#define DEBUG_PUTSTRING(x)	bitbang_putstring(x, sizeof(x))
	#define DEBUG_PUTCHAR(x) 		bitbang_putchar(x)
#else
	#define DEBUG_PUTSTRING(x) 
	#define DEBUG_PUTCHAR(x) 
#endif

void bitbang_putchar(uint8_t c);
void bitbang_putstring(const char* string, int size);
	void Enviar_RS232(uint64_t data);
	uint8_t dec2hex(uint8_t val);
void configure_usart(void);

#endif /* SERIAL_H_ */
