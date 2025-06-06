#ifndef MQ135_H
#define MQ135_H

// inclusão de bibliotecas
#include "pico/stdlib.h"

// definição das funções

// inicializa o ADC e configura o GPIO correspondente ao MQ-135
void mq135_init(void);
// lê o valor bruto (inteiro de 0 a 4095) do ADC conectado ao MQ-135
uint16_t mq135_read_raw(void);
// converte o valor bruto do ADC para a tensão correspondente (0 a 3.3V)
float mq135_read_voltage(uint16_t raw_adc);
// converte o valor bruto do ADC para um valor percentual (0 a 100%)
float mq135_read_percentage(uint16_t raw_adc);
// retorna uma string com a categoria de qualidade do ar, com base na porcentagem calculada
char* mq135_get_category(float percentage);

#endif
