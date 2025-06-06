#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#define MQ135_ADC_INPUT 2 // Canal ADC onde o MQ-135 está conectado
#define ADC_BASE_PIN 28 // Pino base do ADC (pode variar dependendo da placa)

float read_voltage(uint16_t raw_adc) {
    return raw_adc * 5.0f / 4095.0f;  // 12-bit ADC resolução 5.0V
}

// Estimativa simples de qualidade do ar
const char* air_quality_category(float voltage) {
    printf("Voltage %.2f \n", voltage);
    if (voltage < 1.0)
        return "Ar Muito Bom";
    else if (voltage < 1.5)
        return "Ar Bom";
    else if (voltage < 2.0)
        return "Ar Moderado";
    else if (voltage < 2.5)
        return "Ar Ruim";
    else
        return "Ar Muito Ruim";
}

// Lê o valor do MQ-135 e retorna a voltagem correspondente
float read_mq135() {
    adc_select_input(MQ135_ADC_INPUT);
    uint16_t raw_adc = adc_read();
    float voltage = read_voltage(raw_adc);
    return voltage;
}

// Inicializa o MQ-135 configurando o ADC
void mq135_init() {
    adc_init();
    adc_gpio_init(MQ135_ADC_INPUT);
    adc_select_input(MQ135_ADC_INPUT);
}