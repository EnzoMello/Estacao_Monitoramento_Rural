#ifndef WIFI_H
#define WIFI_H

// inclusão de bibliotecas
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"

// wifi credenciais
#define WIFI_SSID "ADRIANA CABELOS"
#define WIFI_PASS "15162324"

// definição das funções
int wifi_connect();

#endif