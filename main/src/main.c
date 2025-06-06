// bibliotecas padrões do C e do SDK do raspberry pi pico w
#include <stdio.h>
#include "pico/stdlib.h"

// bibliotecas utilitárias para os sensores e outros componentes
#include "dht11.h"
#include "mq135.h"



// Função responsável por inicializar os componentes
void setup() {
    
    // inicializando a comunicação serial
    stdio_init_all();

    // inicializando o sensor DHT22
    dht11_init();

    // inicializa o sensor de gás MQ135
    mq135_init();
    
}

int main()
{

    // inicializando os dispositivos
    setup();

    /* ---------> TESTES <--------- */

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

        sleep_ms(1000);
    }
}
