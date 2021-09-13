#ifndef OUTPUT_H_
#define OUTPUT_H_

//Timeout do cartão em ms
#define OUTPUT_CARD_TIMEOUT_MS (300)
#define OUTPUT_CARD_FORCE_OUTPUT_TIMEOUT_MS (3000)

void go_output(uint64_t data, uint8_t size);
void timeout_timer_init(void);
void timeout_timer_start(void);
void timeout_timer_stop(void);

#endif /* OUTPUT_H_ */
