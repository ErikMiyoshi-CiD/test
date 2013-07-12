/*
 * AbaTK2.h
 *
 * Created: 10/07/2013 16:48:46
 *  Author: Neto & David
 */ 

//Como usar:
//
// 1 - Adquirir os dados do cart�o lido.
// 2 - Caso os valores de site code e facility code estejam separados, chamar a Monta_Dados_Cartao_ABATK2() para concaten�-los.
// 3 - Utilizar a fun��o Enviar_ABA_TK2() com os dados concatenados anteriormente ou com os dado obtido do cart�o caso este j� venha unificado
//

#ifndef ABATK2_H_
#define ABATK2_H_

void Enviar_ABA_TK2(uint64_t val);
uint64_t Monta_Dados_Cartao_ABATK2(uint64_t num, int ver);

#endif /* ABATK2_H_ */