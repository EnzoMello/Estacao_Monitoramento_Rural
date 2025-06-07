#ifndef DISPLAY_H
#define DISPLAY_H

#include "pico/stdlib.h"
#include "ssd1306.h" // Controla o display OLED SSD1306 para exibir informações na tela

// Definições do display SSD1306
static ssd1306_t display;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define I2C_SDA 14
#define I2C_SCL 15

void display_init();
void display_write(const char *msg, uint x, uint y, uint size);
void display_show();
void display_clear();

void display_message(const char *message);
void display_data(int temp, int humidty, const char* alert_msg);
void display_initial_screen();

#endif