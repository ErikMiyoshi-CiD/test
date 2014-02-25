/*
 * helper.h
 *
 * Created: 08/11/2013 18:27:55
 *  Author: Luiz
 */ 

#include "sam.h"
#include "RF_common.h"

#ifndef HELPER_H_
#define HELPER_H_

#define USER_INFO_ADD 0x3F00

TIPO_LEITOR ler_tipo_leitor(void);
void ioport_set_pin_level(uint8_t pino, int estado);
void system_flash_set_waitstates(uint8_t wait_states);
int system_gclk_is_syncing(void);
void ClockInit();
void Init125khz(void);
void buzzer_clock_init(void);
void buzz(uint32_t);
void ok_feedback(void);
void system_init(void);
void ioport_set_pin_input(uint8_t pino);
uint8_t ioport_get_pin_level(uint8_t pino);

#endif