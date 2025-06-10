#ifndef WIFI_H
#define WIFI_H

// inclusão de bibliotecas
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"

// wifi credenciais
#define WIFI_SSID "SUA REDE WIFI"
#define WIFI_PASS "SUA SENHA"

// definição das funções
int wifi_connect();

#endif