// inclusions libraries
#include "server.h"

// tcp_pcb struct to stablish connection
struct tcp_pcb *pcb = NULL;

// variables and definition to control connetion attemps
bool have_connection = false;
#define MAX_RETRIES 5
int retries = 0;

// implementation functions


// Callback para a resposta do servidor
err_t server_tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        printf("Connection closed by the server.\n");
        tcp_close(tpcb);
        have_connection = false;
        pcb = NULL;
        return ERR_OK;
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

// Callback para erro na conexão TCP
void server_tcp_client_error(void *arg, err_t err) {
    printf("TCP connection error (%d). Trying to reconnect..\n", err);
    pcb = NULL;
    have_connection = false;
}

// Fechar conexão TCP
void server_close_tcp_connection() {
    if (pcb != NULL) {
        tcp_close(pcb);
        pcb = NULL;
        have_connection = false;
        printf("TCP connection closed.\n");
    }
}

// Callback para sucesso na conexão com o servidor
err_t server_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        printf("Falha ao conectar: %d\n", err);
        tcp_abort(tpcb);
        pcb = NULL;
        return err;
    }

    printf("Conexão estabelecida com sucesso!\n");
    have_connection = true;

    // Define callback de recv aqui
    tcp_recv(tpcb, server_tcp_client_recv);

    return ERR_OK;
}

// Cria conexão com o servidor
void server_create_tcp_connection() {
    if (pcb != NULL) return;    // conexão já existe
    retries++;

    // configuração do servidor
    ip_addr_t server_ip;
    server_ip.addr = ipaddr_addr(SERVER_IP);

    // criando pdb
    pcb = tcp_new();
    if (!pcb) {
        printf("Failed to create pcb\n");
        return;
    }

    tcp_err(pcb, server_tcp_client_error);  // registrando um callback para erro de conexão

    // tentando se conectar ao servidor
    err_t connect_err = tcp_connect(pcb, &server_ip, SERVER_PORT, NULL);
    if (connect_err == ERR_OK) {
        printf("Conected to server!\n");
        have_connection = true;
        tcp_recv(pcb, server_tcp_client_recv);
    } else {
        printf("Connection failed (%d). Attempted %d/%d\n", connect_err, retries, MAX_RETRIES);
        tcp_abort(pcb);
        pcb = NULL;
    }
}

void server_send_data(int temperature, int humidity, float pollutionLevel) {
    // estabelecer uma conexão com o servidor
    while (pcb == NULL || !have_connection) {
        if (retries >= MAX_RETRIES) {
            printf("Error: Connection could not be re-established after %d attempts.\n", MAX_RETRIES);
            return;  // Aborts if the maximum number of reconnection attempts is reached
        }

        printf("Connection lost. Trying to reconnect... (%d/%d)\n", retries, MAX_RETRIES);
        server_create_tcp_connection();  // Attempts to reconnect
        sleep_ms(1000);
    }

    retries = 0;
    
    // Preparando o corpo da requisição
    char json_request[256];
    snprintf(json_request, sizeof(json_request),
    "{\n"
    "\"temperature\" : %d,\n"
    "\"humidity\" : %d,\n"
    "\"pollutionLevel\" : %.1f\n"
    "}", temperature, humidity, pollutionLevel);

    // Montando requisição
    char request[521];
    snprintf(request, sizeof(request),  
    "POST /api/sensors-data HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %d\r\n"
    "\r\n"
    "%s"
    "\r\n",
    SERVER_IP, strlen(json_request), json_request);

    // Escrevendo a requisição
    if (tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY) != ERR_OK) {
        printf("Erro ao enviar dados\n");
        tcp_abort(pcb);
        return;
    }

    // Enviando a requisição
    if (tcp_output(pcb) != ERR_OK) {
        printf("Erro ao enviar dados (tcp_output)\n");
        tcp_abort(pcb);
        return;
    }
}