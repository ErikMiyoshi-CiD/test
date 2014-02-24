/*
 * helper.h
 *
 * Created: 08/11/2013 18:27:55
 *  Author: Luiz
 */ 

#include "sam.h"

#ifndef HELPER_H_
#define HELPER_H_

void ioport_set_pin_level(uint8_t pino, int estado);
void system_flash_set_waitstates(uint8_t wait_states);
int system_gclk_is_syncing(void);
void ClockInit();
void Init125khz(void);
void buzzer_clock_init(void);
void buzz(uint32_t);
void ok_feedback(void);
void system_init(void);

#endif