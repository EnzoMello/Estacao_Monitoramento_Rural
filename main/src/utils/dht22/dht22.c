#include "DHT22.h"
#include <stdio.h>
#include <string.h>


/**
 * @note O DHT22 ele não envia dados espontâneamente, ele depende sempre de um envio de um pulso inicial do microcontrolador
 * para ele começar a enviar seus pulsos de volta. O pulso é uma "chave" para ativar o sensor, é receber dados 
 */


 /**
 * @brief Inicializa o pino GPIO utilizado para comunicação com o sensor DHT22.
 *
 * @note Essa função apenas configura o pino definido por DHT_PIN para que possa ser utilizado 
 * posteriormente nas operações de leitura e envio de pulso. Ela não inicia nenhuma comunicação
 * direta com o sensor, apenas prepara o pino para uso.
 *
 * @warning É importante chamar essa função antes de qualquer operação de leitura ou envio de pulso
 * para garantir que o pino esteja devidamente configurado.
 */
void dht22_init() {
    gpio_init(DHT_PIN);
}

/**
 * @brief Lê a duração de um pulso no pino do sensor DHT22.
 *
 * @param level Nível lógico que deve ser aguardado: true (alto) ou false (baixo).
 * @return Duração do pulso em microssegundos, ou TIMEOUT_DHT em caso de timeout.
 * 
 * @note Após a ativação, o DHT envia seus dados brutos por meio de uma sequência de pulsos que devem ser
 * interpretados pelo microcontrolador(temp e humid). Cada bit de dado é enviado com dois pulsos 
 * e cada um desses pulsos tem sua duração. O microcontrolador interpreta essa duração de cada um 
 * e separa os bits de temp e humid.
 */
static uint32_t dht22_read_pulse(bool level) {  
    uint32_t init = time_us_32();

    // Se o nível atual passar o tempo limite sem mudar, interrompe o loop
    while(gpio_get(DHT_PIN) == level) {
        if(time_us_32() - init > TIMEOUT_DHT) {
            return TIMEOUT_DHT;
        }
    }

    // Se o nível mudar no tempo padrão, calcula o tempo que o DHT passou em um pulso específico
    return time_us_32() - init;
}

/**
 * @brief Envia o pulso de inicialização para o sensor DHT22 e prepara o GPIO para receber os dados dele.
 *
 * @note O microcontrolador precisa envia um pulso baixo de aproximadamente 18 ms seguido por um curto pulso alto.
 *
 */
void dht22_send_pulse_start() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(18);
    gpio_put(DHT_PIN, 1);
    sleep_us(30);
    gpio_set_dir(DHT_PIN, GPIO_IN);
}

/**
 * @brief Lê os dados brutos enviados pelo sensor DHT22.
 *
 *
 * @param data Vetor de 5 bytes onde os dados brutos serão armazenados.
 * @return true se a leitura for bem-sucedida e o checksum estiver correto; false caso contrário.
 * 
 * @note O DHT transmite 40 bits(umidade, temperatura e checksum), que são interpretados e armazenados em um vetor.
 */
bool dht22_read_dht22_data(uint8_t *data) {
    memset(data, 0, 5); // 

    // Se os pulsos de inicialização forem igual ao limite, falha na leitura
    if (dht22_read_pulse(0) == TIMEOUT_DHT || dht22_read_pulse(1) == TIMEOUT_DHT) {
        printf("Error: No sensor DHT22 response");
        return false;
    }


    for (int i = 0; i < 40; i++) {
        // Cada bit de dados, começa pelo pulso baixo e se chegar no tempo limite, falha.
        if (dht22_read_pulse(0) == TIMEOUT_DHT) {
            printf("Error: Low pulse too long");
            return false;
        }

        // O próximo nível é o alto, 
        uint32_t pulse_duration = dht22_read_pulse(1);
        if (pulse_duration == TIMEOUT_DHT) {
            printf("Error: High pulse too long");
            return false;
        }

        // Se a duração do pulso alto for maior que 50 microssegundos, considera 1 e armazena no seu devido local do vetor
        if (pulse_duration > 50) {
            data[i / 8] |= (1 << (7 - (i % 8))); // Cálculo para armazenamento dentro do vetor.
        }
    }

    // Soma dos 4 primeiros bits do vetor deve ser igual ao checksum(bit de verificação) enviado para verificar se a leitura está correta.
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        printf("Erro: Invalid checksum\n");
        return false;
    }

    return true;
}

/**
 * @brief Obtém e converte os dados brutos de temperatura e umidade do DHT22.
 *
 *
 * @param temperature Ponteiro para armazenar a temperatura lida em graus Celsius.
 * @param humidity Ponteiro para armazenar a umidade relativa lida em porcentagem.
 * @return true se a leitura e conversão forem bem-sucedidas; false em caso de erro.
 */
bool dht22_get(float *temperature, float *humidity) {
    
    // Obtém dados brutos e armazena no vetor
    uint8_t data[5];
    if (!dht22_read_dht22_data(data)) {
        return false;  
    }

    // Pega ss dois primeiros bits, que são a humidade e divide por 10 para obter em porcentagem.
    uint16_t raw_humidity = (data[0] << 8) | data[1];
    *humidity = raw_humidity / 10.0;

    // Pega os próximos dois bits, de temperatura.
    uint16_t raw_temperature = (data[2] << 8) | data[3];
    // Verifica se o bit de sinal, o 15, é negativo e converte o sinal caso seja negativo. Depois, divide por 10 para obter a temperatura exata.
    if (raw_temperature & 0x8000) {
        raw_temperature &= 0x7FFF;
        *temperature = (raw_temperature / 10.0) * -1.0;
    } else {
        *temperature = raw_temperature / 10.0;
    }

    return true;
}




