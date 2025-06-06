// bibliotecas padrões do C e do SDK do raspberry pi pico w
#include <stdio.h>
#include "pico/stdlib.h"

// bibliotecas utilitárias para os sensores e outros componentes
#include "dht11.h"
#include "mq135.h"
#include "display.h"
#include "wifi.h"

// função para iniciar a conexão wifi e exibir os feedbacks de conexão no display
int init_wifi_connection() {

    // displaying the name of the wifi network
    display_clear();
    display_write("Conectando em:", 23, 20, 1);
    display_write(WIFI_SSID, 23, 32, 1);
    display_show();

    // trying to connect to the network and retrieving connection status
    int state_connection = wifi_connect();

    // show state of connection on display
    display_clear();
    if (state_connection == 0) {
        display_write("CONEXAO ESTABECIDA", 10, 30, 1);
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    } else {
        display_write("FALHA NA CONEXAO", 10, 30, 1);
    }
    display_show();
    sleep_ms(3000);

    return state_connection;
}

// Função responsável por inicializar os componentes
void setup() {
    
    // inicializando a comunicação serial
    stdio_init_all();

    // inicializando o sensor DHT22
    dht11_init();

    // inicializa o sensor de gás MQ135
    mq135_init();

    // inicializa o display
    display_init();
    
}

int main()
{

    // inicializando os dispositivos
    setup();

    // tentando se conectar a rede wifi
    if (init_wifi_connection() == 0) {
        // conexão realizada com sucesso
    }

    /* ---------> TESTES <--------- */

    // Texto de título para simulação
    char title_message[128] = "";
    int temperature_example;

    // variáveis para armazenar os valores de temperatura e umidade
    int temperature, humidty;
    
    // variáveis para armazenar os resultados em relação a qualidade do ar
    float raw_value;        // valor bruto do adc
    float percent_value;    // conversão para porcentagem  

    while (true) {

        // fazendo leitura do sensor de gás mq135
        raw_value = mq135_read_raw();
        percent_value = mq135_read_percentage(raw_value);
        char *air_status = mq135_get_category(percent_value);
        
        // envia pulso para o dht11 começar a leitura
        dht11_send_pulse_start();

        // fazendo leitura e exibindo os valores no serial monitor
        if (dht11_get(&temperature, &humidty)) {
            // exibe os dados do sensor dht11
            printf("Temperature: %d °C - Humidity: %d \n", temperature, humidty);

            // exibe os dados do sensor de gás mq135
            printf("Qualidade do Ar: %.1f%% - %s\n", percent_value, air_status);

        } else {
            printf("Failed to read dht11 data\n");
        }

        // display_data(10,10, air_quality_category(read_mq135()));

        sleep_ms(1000);
    }
}
