//Como usar:
//
// 1 - Adquirir os dados do cartão lido.
// 2 - Caso os valores de site code e facility code estejam separados, chamar a Monta_Dados_Serial() para concatená-los.
// 3 - Utilizar a função Transmite_Cartao_Serial() com os dados concatenados anteriormente ou com os dado obtido do cartão
//     caso este já venha unificado
//

#ifndef SERIALOUT_H_
#define SERIALOUT_H_

void Transmite_Cartao_Serial(uint64_t val);
uint64_t Monta_Dados_Serial (uint64_t num, int ver);

#endif /* SERIALOUT_H_ */