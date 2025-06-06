#ifndef WIFI_H
#define WIFI_H

// inclusão de bibliotecas
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"

// wifi credenciais
#define WIFI_SSID "sua rede wifi"
#define WIFI_PASS "sua senha"

// definição das funções
int wifi_connect();

#endif