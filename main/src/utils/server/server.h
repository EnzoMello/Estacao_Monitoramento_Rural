#ifndef SERVER_H
#define SERVER_H

// inclusão de bibliotecas
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"

// configuração servidor
#define SERVER_IP "IP SERVIDOR (ipv4 caso esteja rodando localmente)"
#define SERVER_PORT 8080

// definitions functions
void server_create_tcp_connection();
void server_close_tcp_connection();
void server_tcp_client_error(void *arg, err_t err);
err_t server_tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

void server_send_data(int temperature, int humidity, float pollutionLevel);

#endif
