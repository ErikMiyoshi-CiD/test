#include <asf.h>

#include "Pinos.h"
#include "RF_common.h"
#include "RF_ASK.h"
#include "Helper.h"
#include "Output.h"

volatile uint8_t cur_buf;
volatile uint8_t n_samples;
volatile uint8_t semaforo_ask;

static const int _row_good[32] = {
	1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0
};

static inline int count_ones(unsigned int i)
{
	// Calcula de maneira eficiente o numero de
	// bits 1 no valor de entrada.
	// O resultado sera sempre um numero entre 0 e 32.

	i = i - ((i >> 1) & 0x55555555u);
	i = (i & 0x33333333u) + ((i >> 2) & 0x33333333u);
	return (int)((((i + (i >> 4)) & 0x0F0F0F0Fu) * 0x01010101u) >> 24);
}

static void cospe_ask(uint64_t card)
{
	uint8_t site_code;
	uint16_t id;	
	uint32_t dado;
	
	site_code = (((card >> 26u) & 0xfu) << 0u) |
			    (((card >> 31u) & 0xfu) << 4u);
	
	id = (((card >> 6u) & 0xfu) << 0u)   |
		 (((card >> 11u) & 0xfu) << 4u)  |
		 (((card >> 16u) & 0xfu) << 8u)  |
		 (((card >> 21u) & 0xfu) << 12u) ;
	
	dado=((uint32_t)site_code << 16) + id;
	
	go_output(dado);
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
			return;
		}
		temp >>= 5;
	}

	// Verificar paridade das colunas
	col = (((uint64_t)0x00084210)<<32) + (uint64_t)0x84210842;
	
	for (i = 0; i < 4; i++)
	{
		temp = val & col;
		if ((count_ones(temp >> 32) + count_ones((unsigned int)temp)) & 1)
		{
			// Erro na paridade da coluna
			return;
		}
		col <<= 1;
	}
	if( (val & 1) == 1)
	{
		// Erro no stop bit
		return;
	}
	if( (val & ((  ((uint64_t)1) << 55)-1)) == 0 )
	{
		// Erro Cartão "zero"
		return;
	}
	cospe_ask(val);
}

static int pulses=0;
static uint64_t cartao=0;
static uint64_t last_sig_in=0;
static uint64_t sig_in=0;
static int n_bits=0;

static MANCHESTER_STATE _m_state = IDLE;

void Init_Timer(void)
{
	//Habilita o clock do TC3
	PM->APBCMASK.reg |= PM_APBCMASK_TC3;
	
	//Reseta o TC3
	TC3->COUNT16.CTRLA.bit.SWRST = 1;
	while (TC3->COUNT16.CTRLA.bit.SWRST!=0);
	
	//Configura o TC3
	TC3->COUNT16.CTRLA.reg = TC_CTRLA_WAVEGEN_MFRQ; //GCLK/1, não roda em STBY, MFRQ, COUNT16, Disable
	TC3->COUNT16.CTRLBSET.reg = 0; //Count up infinitamente
	TC3->COUNT16.CTRLC.reg = 0; //Sem capture/Compare e não inverte nenhuma saída
	TC3->COUNT16.CC[0].reg = 2047; //Valor para 8MHz é 2048 --> fsample = 3906.25Hz
	
	TC3->COUNT16.INTENSET.bit.OVF = 1; //Habilita OVF interrrupt

	//Habilita o TC3
	//TC3->COUNT16.CTRLA.bit.ENABLE = 1;
}

void ASK_Run(void)
{
	int i,j;

	while(1)
	{
		wdt_reset();
		while(semaforo_ask==0);
		semaforo_ask=0;

		for (i=0;i<ASK_BUF_SIZE;i++)
		{
			if (cur_buf==0)
				sig_in=buf[1][i];
			else
				sig_in=buf[0][i];

			switch(_m_state)
			{
			case IDLE:
				if (sig_in == 0 && last_sig_in == 0)
				_m_state = SYNCHING;
				break;
			case SYNCHING:
				if (sig_in != last_sig_in) //Teve inversão
				pulses++;
				else //Não teve inversão
				{
					if (pulses >= 17) //Achamos sincronismo!
					{
						cartao = 0;
						n_bits = 0;
						for (j = 0; j < ((pulses + 1) / 2); j++)
						{
							n_bits++;
							cartao = cartao << 1;
							cartao = cartao | 1;
						}
						_m_state = DEMOD_TRANS;
					}
					pulses = 0;
				}
				break;
			case DEMOD_WAIT:
				_m_state = DEMOD_TRANS;
				break;
			case DEMOD_TRANS:
				if (sig_in == last_sig_in) //não teve transição onde deveria
				_m_state = IDLE;
				else
				{
					cartao = cartao << 1;
					if (sig_in == 1)
					cartao = cartao | 1;
					else
					cartao = cartao | 0;
					_m_state = DEMOD_WAIT;
					n_bits++;

					if (n_bits == 64)
					{
						processa_resultado(cartao);
						_m_state = IDLE;
					}
				}
				break;
				default:
				_m_state = IDLE;
				break;
			}
			last_sig_in=sig_in;
		}	
	}
}

void ASK_Init(void)
{
	cur_buf=0;
	n_samples=0;
	semaforo_ask=0;
	
	Init125khz();
	NVIC_EnableIRQ(16); //Habilita TC3. Pagina 25 do datasheet
	Init_Timer();
}

uint8_t temp_pin;
void TC3_Handler(void)
{
	TC3->COUNT16.INTFLAG.bit.OVF=1;
	temp_pin=ioport_get_pin_level(PIN_ASK_IN);
	
	if (n_samples > 127)
	{
		semaforo_ask=1;
		n_samples=0;
		
		cur_buf++;
		if (cur_buf>1)
			cur_buf=0;
	}
	buf[cur_buf][n_samples]=temp_pin;
	
	n_samples++;
}