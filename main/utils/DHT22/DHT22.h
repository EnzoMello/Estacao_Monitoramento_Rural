#ifndef DHT22_H
#define DHT22_H

// PINO PARA O DHT
#define DHT_PIN 14

// TIME PARA LEITURA DOS PULSOS
#define TIMEOUT_DHT 1000

void send_pulse_start();
bool read_dht22_data(uint8_t *data);
bool get_dht22(float *temperature, float *humidity);

#endif