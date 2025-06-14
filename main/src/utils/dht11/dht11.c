#include "DHT11.h"
#include <stdio.h>
#include <string.h>


/**
 * @note O DHT11 ele não envia dados espontâneamente, ele depende sempre de um envio de um pulso inicial do microcontrolador
 * para ele começar a enviar seus pulsos de volta. O pulso é uma "chave" para ativar o sensor, é receber dados 
 */


 /**
 * @brief Inicializa o pino GPIO utilizado para comunicação com o sensor DHT11.
 *
 * @note Essa função apenas configura o pino definido por DHT_PIN para que possa ser utilizado 
 * posteriormente nas operações de leitura e envio de pulso. Ela não inicia nenhuma comunicação
 * direta com o sensor, apenas prepara o pino para uso.
 *
 * @warning É importante chamar essa função antes de qualquer operação de leitura ou envio de pulso
 * para garantir que o pino esteja devidamente configurado.
 */
void dht11_init() {
    gpio_init(DHT_PIN);
}

/**
 * @brief Lê a duração de um pulso no pino do sensor DHT11.
 *
 * @param level Nível lógico que deve ser aguardado: true (alto) ou false (baixo).
 * @return Duração do pulso em microssegundos, ou TIMEOUT_DHT em caso de timeout.
 * 
 * @note Após a ativação, o DHT envia seus dados brutos por meio de uma sequência de pulsos que devem ser
 * interpretados pelo microcontrolador(temp e humid). Cada bit de dado é enviado com dois pulsos 
 * e cada um desses pulsos tem sua duração. O microcontrolador interpreta essa duração de cada um 
 * e separa os bits de temp e humid.
 */
static uint32_t dht11_read_pulse(bool level) {  
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
 * @brief Envia o pulso de inicialização para o sensor DHT11 e prepara o GPIO para receber os dados dele.
 *
 * @note O microcontrolador precisa envia um pulso baixo de aproximadamente 18 ms seguido por um curto pulso alto.
 *
 */
void dht11_send_pulse_start() {
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(18);
    gpio_put(DHT_PIN, 1);
    sleep_us(30);
    gpio_set_dir(DHT_PIN, GPIO_IN);
}

/**
 * @brief Lê os dados brutos enviados pelo sensor DHT11.
 *
 *
 * @param data Vetor de 5 bytes onde os dados brutos serão armazenados.
 * @return true se a leitura for bem-sucedida e o checksum estiver correto; false caso contrário.
 * 
 * @note O DHT transmite 40 bits(umidade, temperatura e checksum), que são interpretados e armazenados em um vetor.
 */
bool dht11_read_dht11_data(uint8_t *data) {
    memset(data, 0, 5); // 

    // Se os pulsos de inicialização forem igual ao limite, falha na leitura
    if (dht11_read_pulse(0) == TIMEOUT_DHT || dht11_read_pulse(1) == TIMEOUT_DHT) {
        printf("Error: No sensor DHT11 response");
        return false;
    }


    for (int i = 0; i < 40; i++) {
        // Cada bit de dados, começa pelo pulso baixo e se chegar no tempo limite, falha.
        if (dht11_read_pulse(0) == TIMEOUT_DHT) {
            printf("Error: Low pulse too long");
            return false;
        }

        // O próximo nível é o alto, 
        uint32_t pulse_duration = dht11_read_pulse(1);
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
 * @brief Obtém e converte os dados brutos de temperatura e umidade do DHT11.
 *
 *
 * @param temperature Ponteiro para armazenar a temperatura lida em graus Celsius.
 * @param humidity Ponteiro para armazenar a umidade relativa lida em porcentagem.
 * @return true se a leitura e conversão forem bem-sucedidas; false em caso de erro.
 */
bool dht11_get(int *temperature, int *humidity) {
    
    // Obtém dados brutos e armazena no vetor
    uint8_t data[5];

    if (!dht11_read_dht11_data(data)) {
        return false;  
    }

    *humidity = data[0];
    *temperature = data[2];  

    return true;
}

/**
 * @brief Categoriza a temperatura em níveis descritivos de acordo com o valor lido.
 *
 * @param temperature Temperatura lida pelo sensor DHT11 em graus Celsius.
 * @return Ponteiro para uma string constante contendo a descrição da categoria:
 *         - "Muito Frio"      (temperature < 10)
 *         - "Frio"            (10 <= temperature < 20)
 *         - "Agradável"       (20 <= temperature < 30)
 *         - "Quente"          (30 <= temperature < 35)
 *         - "Muito Quente"    (temperature >= 35)
 *
 * @note Essa função permite associar um valor numérico de temperatura a uma descrição textual,
 * facilitando a interpretação dos dados em sistemas embarcados ou interfaces de usuário.
 */
char *dht11_get_temperature_category(int temperature) {
    if (temperature < 10)
        return "Muito Frio";
    else if (temperature < 20)
        return "Frio";
    else if (temperature < 30)
        return "Agradavel";
    else if (temperature < 35)
        return "Quente";
    else
        return "Muito Quente";
}

/**
 * @brief Categoriza a umidade relativa em níveis descritivos de acordo com o valor lido.
 *
 * @param humidity Umidade relativa lida pelo sensor DHT11 em porcentagem (%).
 * @return Ponteiro para uma string constante contendo a descrição da categoria:
 *         - "Seco"            (humidity < 30)
 *         - "Confortável"     (30 <= humidity < 60)
 *         - "Úmido"           (60 <= humidity < 80)
 *         - "Muito Úmido"     (humidity >= 80)
 *
 * @note Essa função permite associar um valor numérico de umidade a uma descrição textual,
 * facilitando a interpretação dos dados ambientais pelo sistema ou pelo usuário.
 */
char *dht11_get_humidity_category(int humidity) {
    if (humidity < 30)
        return "Seco";
    else if (humidity < 60)
        return "Confortavel";
    else if (humidity < 80)
        return "Umido";
    else
        return "Muito umido";
}
