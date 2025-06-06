#include "wifi.h"

// implementação das funções

int wifi_connect() {

    int connected;
    // inicialização do chip
    if (cyw43_arch_init()) {
        printf("Wi-fi init failed.\n");
        return 1;
    }

    // ativa o modo cliente do wifi
    cyw43_arch_enable_sta_mode();

    // tentando se conectar em até 10s
    printf("Connecting Wifi...\n");
    connected = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000);
    
    // exibe status de conexão no terminal e retorna o estado
    char *message_log = connected == 0? "Wifi conected!\n" : "Failed to connect.\n";
    printf(message_log);
    return connected;
}