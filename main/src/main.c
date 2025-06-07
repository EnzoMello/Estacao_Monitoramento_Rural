// bibliotecas padrões do C e do SDK do raspberry pi pico w
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "string.h"

// bibliotecas utilitárias para os sensores e outros componentes
#include "dht11.h"
#include "mq135.h"
#include "display.h"
#include "wifi.h"
#include "button.h"

// máquina de estados para a aplicação
typedef enum {
    IDLE_STATE,
    SENSOR_READING_STATE,
    DISPLAY_DATA_STATE
} StateMachine;

// instância da máquina de estado para a aplicação
StateMachine global_state;

// estrutura para armazenar os dados dos sensores
typedef struct {
    int temperature;
    int humidity;
    float pollutionLevel;
    
    char temperatureCategory[100];
    char humidityCategory[100];
    char airQualityCategory[100];
} SensorData;

// instância global para armazenar os dados dos sensores
SensorData *global_sensor_data = NULL;

// variáveis de controle
bool button_is_active = false;

// estrutura de timer para tratar o efeito bounce do botão
struct repeating_timer button_debouncing_timer;

// função para iniciar a conexão wifi e exibir os feedbacks de conexão no display
int init_wifi_connection() {

    // exibindo o nome da rede wifi
    display_clear();
    display_write("Conectando em:", 23, 20, 1);
    display_write(WIFI_SSID, 23, 32, 1);
    display_show();

    // tentando se conectar com a rede e obtendo o status de conexão
    int state_connection = wifi_connect();

    // exibindo o estado de conexão no display
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

// função para obter e armazenar as mensagens de alerta com base nos dados dos sensores
void get_and_store_alerts_message(SensorData *data) {
    // mensagem de temperatura
    strcpy(
        data->temperatureCategory, 
        dht11_get_temperature_category(
            data->temperature
        )
    );

    // mensagem de umidade
    strcpy(
        data->humidityCategory, 
        dht11_get_humidity_category(
            data->humidity
        )
    );

    // mensagem de qualidade do ar
    strcpy(
        data->airQualityCategory, 
        mq135_get_category(
            data->pollutionLevel
        )
    );
}

// callback para reativar o botão
bool reenable_button_callback() {
    button_is_active = true;
    return false;
}

// função para tratar o efeito bounce
void debouncing() {
    button_is_active = false;
    cancel_repeating_timer(&button_debouncing_timer);
    add_repeating_timer_ms(300, reenable_button_callback, NULL, &button_debouncing_timer);
}

// callback para tratar as interrupções dos pinos GPIO
void gpio_irq_callback(uint gpio, uint32_t event) {

    // caso o botão não esteja ativo
    if (!button_is_active) return;

    // TODO: criar uma função para tratar o efeito bounce
    debouncing();

    // caso pressione o botão B esteja no estado inicial da alicaçaõ
    if(global_state == IDLE_STATE) {
        // muda o estado da aplicação para o modo leitura dos sensores
        global_state = SENSOR_READING_STATE;
    }
    
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
    
    // iniciando o botão B e configurando interrupção
    button_init();
    button_is_active = true;
    gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_callback); 

    // alocando memória para a estrutura que armazena os dados dos sensores
    global_sensor_data = (SensorData*) malloc(sizeof(SensorData));
}

int main()
{

    // inicializando os dispositivos
    setup();

    // tentando se conectar a rede wifi e caso a conexão coma rede falhar ele finalza a aplicação
    if (init_wifi_connection() == 1) return 1;

    // carrega tela inicial
    display_initial_screen();

    // configurando o estado da aplicação para o estado inicial
    global_state = IDLE_STATE;  

    while (true) {

        // caso tenha mudado o estado da aplicação para a de leitura dos sensores
        if (global_state == SENSOR_READING_STATE) {

            // lendo os dados de temperatura e umidade
            // envia pulso inicial para o dht11
            dht11_send_pulse_start();

            // obtenho e armazeno os dados de temperatura e umidade
            bool reading_status = dht11_get(&global_sensor_data->temperature, &global_sensor_data->humidity);
            if (!reading_status) {
                printf("\nFalha ao ler os dados no DHT11");
            }
            
            // obtendo e armazenando os dados de concentração de gases no sensor de gás MQ135
            int raw_value = mq135_read_raw();       // valor bruto ADC
            global_sensor_data->pollutionLevel = mq135_read_percentage(raw_value);  // valor convertido para porcentagem

            // TODO: envia os dados para o servidor

            // armazenando as mensagens de alerta com base nos dados
            get_and_store_alerts_message(global_sensor_data);

            // exibindo os dados lidos no terminal
            printf("\n==== DADOS DOS SENSORES ====\n");
            printf("Temperatura: %d °C\n", global_sensor_data->temperature);
            printf("Categoria de Temperatura: %s\n", global_sensor_data->temperatureCategory);

            printf("Umidade: %d %%\n", global_sensor_data->humidity);
            printf("Categoria de Umidade: %s\n", global_sensor_data->humidityCategory);

            printf("Qualidade do Ar (Poluição): %.2f %%\n", global_sensor_data->pollutionLevel);
            printf("Categoria de Qualidade do Ar: %s\n", global_sensor_data->airQualityCategory);
            printf("============================\n");

            // TODO: exibe os dados localmente no display
            // muda o estado da aplicação para o estado de exibição dos dados locais
            global_state = DISPLAY_DATA_STATE;

            // ao finalizar esse ciclo, coloca o estado novamente para o inicial (IDLE)
            global_state = IDLE_STATE;
            
        }

        sleep_ms(1000);
    }
}
