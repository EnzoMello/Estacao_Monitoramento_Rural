#ifdef MQ135_H
#define MQ135_H

#include "pico/stdlib.h"

float read_voltage(uint16_t raw_adc); // Função para ler a tensão do ADC e retornar em volts
const char* air_quality_category(float voltage); // Função para categorizar a qualidade do ar com base na tensão
float read_mq135(); // Função para ler o MQ-135 e a voltagem correspondente
void mq135_init(); // Função para inicializar o MQ-135

#endif