#include "button.h"

// implementação das funções
void button_init() {
    // iniciando botão B
    gpio_init(BTN_B);
    gpio_set_dir(BTN_B, GPIO_IN);
    gpio_pull_up(BTN_B);
}