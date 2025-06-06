#ifndef DHT11_H
#define DHT11_H

// inclusão de bibliotecas
#include "pico/stdlib.h"

// PINO PARA O DHT
#define DHT_PIN 18

// TIME PARA LEITURA DOS PULSOS
#define TIMEOUT_DHT 1000

void dht11_init();
void dht11_send_pulse_start();
bool dht11_read_dht11_data(uint8_t *data);
bool dht11_get(int *temperature, int *humidity);

#endif