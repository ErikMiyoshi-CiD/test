#include <asf.h>

#include "Pinos.h"
#include "RF_common.h"
#include "RF_ASK.h"
#include "Helper.h"
#include "Output.h"
#include "SerialOut.h"

static volatile uint8_t cur_buf;
static volatile uint16_t n_samples;
static volatile uint8_t semaforo_ask;

static struct tc_module tc_instance;

static uint8_t buf[ASK_BUF_COUNT][ASK_PERIODS][ASK_BUF_SIZE];

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
		
	return;
}

static void TC3_callback(struct tc_module *const module_inst)
{
	uint8_t temp_pin=ioport_get_pin_level(PIN_ASK_IN);
	if ( n_samples > (ASK_PERIODS*ASK_BUF_SIZE-1) ) 
	{
		semaforo_ask=1;
		n_samples=0;
		
		cur_buf++;
		if (cur_buf>1)
			cur_buf=0;
	}
	buf[cur_buf][n_samples % ASK_PERIODS][n_samples/ASK_PERIODS]=temp_pin;
	n_samples++;
}

void Init_Timer(void)
{
	struct tc_config config_tc;
    tc_get_config_defaults(&config_tc);
	config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
    config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
    config_tc.counter_16_bit.compare_capture_channel[0] = 12288/2 - 1; // -> 3906.25 * 2Hz (pegaremos dois pontos deslocados de T/2!)
    tc_init(&tc_instance, TC3, &config_tc);
}

static void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance, TC3_callback, TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_instance, TC_CALLBACK_CC_CHANNEL0);
	tc_enable(&tc_instance);
}

void ASK_Run(void)
{
	int pulses[ASK_PERIODS];
	uint64_t cartao[ASK_PERIODS];
	MANCHESTER_STATE _m_state[ASK_PERIODS];
	uint64_t last_sig_in[ASK_PERIODS];
	uint64_t sig_in[ASK_PERIODS];
	int n_bits[ASK_PERIODS];
	int i,j,k;
	
	for (i=0;i<ASK_PERIODS;i++)
	{
		pulses[i]=0;
		cartao[i]=0;
		_m_state[i] = IDLE;
		last_sig_in[i]=0;
		sig_in[i]=0;
		n_bits[i]=0;	
	}

	while(1)
	{
		wdt_reset_count();
		while(semaforo_ask==0);
		wdt_reset_count();
		semaforo_ask=0;

		for (j=0;j<ASK_PERIODS;j++)
		{
			for (i=0;i<ASK_BUF_SIZE;i++)
			{
				if (cur_buf==0)
					sig_in[j]=buf[1][j][i];
				else
					sig_in[j]=buf[0][j][i];

				switch(_m_state[j])
				{
					case IDLE:
						if (sig_in[j] == 0 && last_sig_in[j] == 0)
							_m_state[j] = SYNCHING;
						break;
					case SYNCHING:
						if (sig_in[j] != last_sig_in[j]) //Teve inversão
							pulses[j]++;
						else //Não teve inversão
						{
							if (pulses[j] >= 17) //Achamos sincronismo!
							{
								cartao[j] = 0;
								n_bits[j] = 0;
								for (k = 0; k < ((pulses[j] + 1) / 2); k++)
								{
									n_bits[j]++;
									cartao[j] = cartao[j] << 1;
									cartao[j] = cartao[j] | 1;
								}
								_m_state[j] = DEMOD_TRANS;
							}
							pulses[j] = 0;
						}
						break;
					case DEMOD_WAIT:
						_m_state[j] = DEMOD_TRANS;
						break;
					case DEMOD_TRANS:
						if (sig_in[j] == last_sig_in[j]) //não teve transição onde deveria
							_m_state[j] = IDLE;
						else
						{
							cartao[j] = cartao[j] << 1;
							if (sig_in[j] == 1)
								cartao[j] = cartao[j] | 1;
							else
								cartao[j] = cartao[j] | 0;
							_m_state[j] = DEMOD_WAIT;
							n_bits[j]++;

							if (n_bits[j] == 64)
							{
								processa_resultado(cartao[j]);
								_m_state[j] = IDLE;
							}
						}
						break;
					default:
						_m_state[j] = IDLE;
						break;
				}
				last_sig_in[j]=sig_in[j];
			}
		}	
	}
}

void ASK_Init(void)
{
	wdt_reset_count();
	
	cur_buf=0;
	n_samples=0;
	semaforo_ask=0;
	
	Init125khz();
	Init_Timer();
	configure_tc_callbacks();
	
	wdt_reset_count();
}

