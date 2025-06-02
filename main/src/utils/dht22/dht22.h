#ifndef DHT22_H
#define DHT22_H

// inclusão de bibliotecas
#include "pico/stdlib.h"

// PINO PARA O DHT
#define DHT_PIN 14

// TIME PARA LEITURA DOS PULSOS
#define TIMEOUT_DHT 1000

void dht22_init();
void dht22_send_pulse_start();
bool dht22_read_dht22_data(uint8_t *data);
bool dht22_get(float *temperature, float *humidity);

#endif