#include "mq135.h"
#include "hardware/adc.h"

// Define qual canal ADC o MQ-135 está conectado (ADC2 no GPIO28)
#define MQ135_ADC_INPUT 2

// Define a tensão de referência do ADC (3.3V para o Raspberry Pi Pico W)
#define ADC_REF_VOLTAGE 3.3f

// Define a resolução máxima do ADC (12 bits → 0 a 4095)
#define ADC_RESOLUTION 4095.0f

// Função para inicializar o ADC e configurar o pino GPIO usado pelo MQ-135
void mq135_init(void) {
    adc_init();                         // Inicializa o ADC
    adc_gpio_init(28);                  // Configura o GPIO28 (canal ADC2) como entrada analógica
    adc_select_input(MQ135_ADC_INPUT);  // Seleciona o canal ADC2 como ativo
}

// Função que realiza a leitura do valor bruto (0 a 4095) do ADC conectado ao MQ-135
uint16_t mq135_read_raw(void) {
    adc_select_input(MQ135_ADC_INPUT); // Garante que o canal ADC correto está selecionado
    return adc_read();                 // Retorna a leitura analógica bruta
}

// Função que converte o valor bruto lido pelo ADC em tensão (0 a 3.3V)
float mq135_read_voltage(uint16_t raw_adc) {
    return (raw_adc * ADC_REF_VOLTAGE) / ADC_RESOLUTION;
}

// Função que converte o valor bruto do ADC em um valor percentual de 0 a 100%
// Esse percentual é relativo à faixa de 0V a 3.3V
float mq135_read_percentage(uint16_t raw_adc) {
    float voltage = mq135_read_voltage(raw_adc);                // Converte para tensão
    float percentage = (voltage / ADC_REF_VOLTAGE) * 100.0f;    // Calcula percentual
    return percentage;
}

// Função que categoriza a qualidade do ar de acordo com o valor percentual obtido
char* mq135_get_category(float percentage) {
    if (percentage < 20.0f)
        return "Ar Muito Bom";
    else if (percentage < 40.0f)
        return "Ar Bom";
    else if (percentage < 60.0f)
        return "Ar Moderado";
    else if (percentage < 80.0f)
        return "Ar Ruim";
    else
        return "Ar Muito Ruim";
}
