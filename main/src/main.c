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

    mq135_init();

    
}

int main()
{

    // inicializando os dispositivos
    setup();

    /* ---------> TESTES <--------- */

    // variáveis para armazenar os valores de temperatura e umidade
    int temperature, humidty;

    

    while (true) {
        float voltage = read_mq135();
        printf("Qualidade: %s \n",air_quality_category(voltage));

        dht11_send_pulse_start();


        air_quality_category(read_mq135());

        // fazendo leitura e exibindo os valores no serial monitor
        if (dht11_get(&temperature, &humidty)) {
            //printf("Temperature: %d °C - Humidity: %d \n", temperature, humidty);

        } else {
            //printf("Failed to read dht11 data\n");
        }

        sleep_ms(1000);
    }
}
