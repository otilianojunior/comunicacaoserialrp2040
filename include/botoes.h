#ifndef BOTOES_H
#define BOTOES_H

#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Pinos dos botões
#define BOTAO_A_PIN 5
#define BOTAO_B_PIN 6

// Atraso para debounce (ms)
#define ATRASO_DEBOUNCE_MS 200

// Variáveis globais para controle dos botões
extern volatile uint32_t tempo_ultimo_interrupcao;
extern volatile bool led_verde_aceso;
extern volatile bool led_azul_aceso;
extern volatile int id_numero;

// Protótipos das funções do módulo de botões
void inicializar_botoes(void);
void callback_botao(uint gpio, uint32_t eventos);

#endif // BOTOES_H
