#ifndef SERIAL_H_
#define SERIAL_H_

#include <asf.h>

#if 0
#define DEBUG_PUTSTRING(x) bitbang_putstring(x,sizeof(x))
#define DEBUG_PUTCHAR(x) bitbang_putchar(x)
#else
#define DEBUG_PUTSTRING(x) 
#define DEBUG_PUTCHAR(x) 
#endif

void bitbang_putchar(uint8_t c);
void bitbang_putstring(const char* string, int size);
void configure_usart(void);

#endif /* SERIAL_H_ */