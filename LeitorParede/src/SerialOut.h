/*
 * SerialOut.h
 *
 * Created: 11/07/2013 17:12:43
 *  Author: ENGENHARIA2
 */ 


#ifndef SERIALOUT_H_
#define SERIALOUT_H_

void Transmite_Cartao_Serial(uint64_t val);
uint64_t Monta_Dados_Serial (uint64_t num, int ver);

#endif /* SERIALOUT_H_ */