/*
 * ASK_Decode.c
 *
 * Created: 04/07/2013 19:35:10
 *  Author: Neto & David
 */

#include <asf.h>
#include "ASK_Decode.h"
#include "config_board.h"
#include "stdio.h"
#include "string.h"
#include "ABATK2.h"
#include "Wiegand.h"
#include "SerialOut.h"

static inline void reinicia_leitura(void);
static inline void leu_bit(int bit);
static inline void processa_resultado(uint64_t val);
static inline int count_ones(uint32_t i);

static volatile int _primeira_vez = 1;
static int _valendo = 0;
static int _uns_consecutivos = 0;

typedef enum
{
	ST_IDLE,
	ST_DECODE,
	ST_DECODE_SHORT,
	ST_ERROR
} manchester_state;

static manchester_state _state = ST_IDLE;

static const int _row_good[32] = {
	1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0
};

static uint64_t num;
static volatile unsigned int cartao_RFID;
static int ver;

void ASK_Decoding(void)
{
	static int cur_bit;
	int duracao;
	int sig_in;

	duracao = tc45_read_count(&TCC5);
	if (duracao < GLITCH_TIME)
		return;

	// Reset counter
	Reinicia_Contagem_TC5();
	delay_us(40);

	if (_primeira_vez)
	{
		_primeira_vez = 0;
		return;
	}

	if (TCC5.INTFLAGS & (1<<0))
	{
		duracao = 65535;
		TCC5.INTFLAGS = (1<<0);
	}
	
	sig_in = ioport_get_pin_level(ASK_FSK);
	
	if (sig_in)
		sig_in=0;
	else
		sig_in=1;

	switch (_state)
	{
		case ST_IDLE:
		// Esperar pulso longo
		if (duracao >= MIN_LONG_TIME &&
		duracao <= MAX_LONG_TIME)
		{
			reinicia_leitura();
			cur_bit = sig_in;
			leu_bit(cur_bit);
			_state = ST_DECODE;
		}
		break;
		case ST_DECODE:
			if (duracao >= MIN_SHORT_TIME && duracao <= MAX_SHORT_TIME)
			{
				// Pulso pequeno, tempo ate proxima
				// borda deve ser pequeno tambem.
				_state = ST_DECODE_SHORT;
			}
			else if (duracao >= MIN_LONG_TIME && duracao <= MAX_LONG_TIME)
			{
				// OK, pegou mais um bit
				cur_bit = !cur_bit;
				leu_bit(cur_bit);
			}
			else
			{
				// Erro!
				_state = ST_IDLE;
			}
			break;
		case ST_DECODE_SHORT:
			if (duracao >= MIN_SHORT_TIME &&
			duracao <= MAX_SHORT_TIME)
			{
				// OK, pegou mais um bit
				leu_bit(cur_bit);
				_state = ST_DECODE;
			}
			else
			{
				// Erro!
				_state = ST_IDLE;
			}
		break;
		default:
			_state = ST_IDLE;
		break;
	}
}

static inline void reinicia_leitura(void)
{
	_valendo = 0;
	_uns_consecutivos = 0;
}

static inline void leu_bit(int bit)
{
	static uint64_t val;
	static int num_bits;
	
	if (!_valendo)
	{
		if (bit)
		{
			if (++_uns_consecutivos >= 9)
			{
				// Achou sincronismo!
				_uns_consecutivos = 0;
				_valendo = 1;
				val = (1u << 9) - 1u; // 9 bits '1'
				num_bits = 9;
			}
		}
		else
			_uns_consecutivos = 0;
	}
	else
	{
		
		val <<= 1;
		if (bit)
			val |= 1u;
		if (++num_bits >= 64)
		{
			// Acabou!
			processa_resultado(val);
			_valendo = 0;
		}
	}
}

static void processa_resultado(uint64_t val)
{
	uint64_t col;
	uint64_t temp;

	int i;
	
	// Verificar paridade das linhas
	temp = val >> 5;
	for (i = 0; i < 10; i++)
	{
		int row = (int)((unsigned int)temp & 0x1fu);
		if (!_row_good[row])
		{
			// Erro na paridade dessa linha
			//usart_serial_write_packet(USART_SERIAL,(const uint8_t*)"!P:R\n\r",strlen("!P:R\n\r"));
			//printf(RFID_MSG "Paridade linha %d\n\r", i);
			return;
		}
		temp >>= 5;
	}

	// Verificar paridade das colunas
	col = COL0;
	for (i = 0; i < 4; i++)
	{
		temp = val & col;
		if ((count_ones(temp >> 32) + count_ones((unsigned int)temp)) & 1)
		{
			// Erro na paridade da coluna
			//printf(RFID_MSG "Paridade coluna %d\n\r", i);
			//usart_serial_write_packet(USART_SERIAL,(const uint8_t*)"!P:C\n\r",strlen("!P:R\n\r"));
			return;
		}
		col <<= 1;
	}

	num =
	((val >> 6u) & 0xfu) |
	(((val >> 11u) & 0xfu) << 4u) |
	(((val >> 16u) & 0xfu) << 8u) |
	(((val >> 21u) & 0xfu) << 12u) |
	(((val >> 26u) & 0xfu) << 16u) |
	(((val >> 31u) & 0xfu) << 20u) |
	(((val >> 36u) & 0xfu) << 24u) |
	(((val >> 41u) & 0xfu) << 28u);
	ver = (int)(
	((val >> 46u) & 0xfu) |
	(((val >> 51u) & 0xfu) << 4u));
	
	//Transmite_Pacote_Wiegand(num);

	//char Hex_Num[20];
	//
	//usart_serial_write_packet(USART_SERIAL,(const uint8_t*)"ASK: ",strlen("ASK: "));
	//
	//sprintf(&Hex_Num[0],"0x%03X",(const uint16_t)((num >> 16) & 0xFFF));
	//sprintf(&Hex_Num[5],"%04X\n\r",(const uint16_t)((num >> 0) & 0xFFFFFFFF));
	//usart_serial_write_packet(USART_SERIAL,(const uint8_t*)Hex_Num,strlen(Hex_Num));
	
	uint64_t serial;
	
	serial = Monta_Dados_Serial(num,ver);
	
	Transmite_Cartao_Serial(serial);
	
}

static inline int count_ones(uint32_t i)
{
	// Calcula de maneira eficiente o numero de
	// bits 1 no valor de entrada.
	// O resultado sera sempre um numero entre 0 e 32.

	i = i - ((i >> 1) & 0x55555555u);
	i = (i & 0x33333333u) + ((i >> 2) & 0x33333333u);
	return (int)((((i + (i >> 4)) & 0x0F0F0F0Fu) * 0x01010101u) >> 24);
}