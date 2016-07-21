#ifndef OUTPUT_H_
#define OUTPUT_H_

//Timeout do cartão em ms
#define OUTPUT_CARD_TIMEOUT (500)
#define OUTPUT_CARD_TIMEOUT_CYCLES (8000000.0/1024.0*OUTPUT_CARD_TIMEOUT/1000.0)

void go_output(uint32_t data);
void timeout_timer_init(void);
void timeout_timer_start(void);
void timeout_timer_stop(void);

#endif /* OUTPUT_H_ */