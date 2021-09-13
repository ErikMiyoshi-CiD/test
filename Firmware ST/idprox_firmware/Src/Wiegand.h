#ifndef WIEGAND_H_
#define WIEGAND_H_

typedef struct {
	uint8_t	leading_parity;
	uint8_t trailing_parity;
	uint64_t data;
} WiegandFrame;

void TxWiegandPacket(uint64_t card_num, uint8_t size);

#endif /* WIEGAND_H_ */
