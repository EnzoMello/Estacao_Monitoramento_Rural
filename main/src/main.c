// bibliotecas padrões do C e do SDK do raspberry pi pico w
#include <stdio.h>
#include "pico/stdlib.h"

// bibliotecas utilitárias para os sensores e outros componentes
#include "dht22.h"


// Função responsável por inicializar os componentes
void setup() {
    
    // inicializando a comunicação serial
    stdio_init_all();

    // inicializando o sensor DHT22
    dht22_init();
}

int main()
{

    // inicializando os dispositivos
    setup();

    /* ---------> TESTES <--------- */

    // variáveis para armazenar os valores de temperatura e umidade
    float temperature, humidty;

    while (true) {

        // fazendo leitura e exibindo os valores no serial monitor
        if (dht22_get(&temperature, &humidty)) {
            printf("Temperature: %.2f - Humidity: %.1f% \n", temperature, humidty);
        } else {
            printf("Failed to read dht22 data\n");
        }

        sleep_ms(1000);
    }
}
