#ifndef LEDS_H
#define LEDS_H

#include "pico/stdlib.h"
#include "hardware/pio.h"

// Configurações da matriz de LEDs
#define NUM_LEDS_MTX 25
#define NIVEL_LED_MTX 51
#define PIN_LED_MTX 7

extern const uint PIN_LED_VERDE;
extern const uint PIN_LED_AZUL;

// Estrutura para armazenar a cor dos LEDs (formato GRB)
typedef struct {
    uint8_t R, G, B;
} led_t;

// Variáveis globais do módulo de LEDs
extern led_t matriz_leds[NUM_LEDS_MTX];
// Padrões numéricos (0-9) em matriz 5x5
extern const uint8_t padrao_numeros_led[10][5][5];

// Protótipos das funções de controle dos LEDs
void definir_led(uint id, uint8_t R, uint8_t G, uint8_t B);
void limpar_leds(void);
uint32_t valor_rgb(uint8_t B, uint8_t R, uint8_t G);
void escrever_leds(void);
void definir_led_por_padrao(const uint8_t padrao[5][5]);

#endif // LEDS_H
