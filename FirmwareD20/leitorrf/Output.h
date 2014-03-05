/*
 * Output.h
 *
 * Created: 2014-03-05 8:34:27 PM
 *  Author: Alb
 */ 


#ifndef OUTPUT_H_
#define OUTPUT_H_

//Timeout do cartão em ms
#define OUTPUT_CARD_TIMEOUT (1000)
#define OUTPUT_CARD_TIMEOUT_CYCLES (8000000.0F/OUTPUT_CARD_TIMEOUT/1000.0F)

void go_output(uint32_t data);

#endif /* OUTPUT_H_ */