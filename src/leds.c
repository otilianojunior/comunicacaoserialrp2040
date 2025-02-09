#include "leds.h"

// Buffer de LEDs e padrões numéricos (0-9) em matriz 5x5
led_t matriz_leds[NUM_LEDS_MTX];

const uint8_t padrao_numeros_led[10][5][5] = {
    [0] = { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0} },
    [1] = { {0, 0, 1, 0, 0}, {0, 1, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 1, 1, 1, 0} },
    [2] = { {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0} },
    [3] = { {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0} },
    [4] = { {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0} },
    [5] = { {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 1, 1, 1, 0} },
    [6] = { {0, 1, 1, 1, 0}, {0, 1, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0} },
    [7] = { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0} },
    [8] = { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0} },
    [9] = { {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 0}, {0, 0, 0, 1, 0}, {0, 0, 0, 1, 0} }
};

void definir_led(uint id, uint8_t R, uint8_t G, uint8_t B) {
    matriz_leds[id].R = R;
    matriz_leds[id].G = G;
    matriz_leds[id].B = B;
}

void limpar_leds(void) {
    for (uint i = 0; i < NUM_LEDS_MTX; i++) {
        matriz_leds[i].R = 0;
        matriz_leds[i].G = 0;
        matriz_leds[i].B = 0;
    }
}

uint32_t valor_rgb(uint8_t B, uint8_t R, uint8_t G) {
    return (G << 24) | (R << 16) | (B << 8);
}

// 'pio' e 'sm' são definidos na main
extern PIO pio;
extern uint sm;

void escrever_leds(void) {
    uint32_t valor;
    for (uint i = 0; i < NUM_LEDS_MTX; ++i) {
        valor = valor_rgb(matriz_leds[i].B, matriz_leds[i].R, matriz_leds[i].G);
        pio_sm_put_blocking(pio, sm, valor);
    }
}

void definir_led_por_padrao(const uint8_t padrao[5][5]) {
    uint nivel = NIVEL_LED_MTX;
    int idx = 0;
    // Varre as linhas de baixo para cima com varredura em ziguezague
    for (int linha = 4; linha >= 0; linha--) {
        bool reverso = (linha % 2 == 0);
        if (reverso) {
            for (int coluna = 4; coluna >= 0; coluna--) {
                if (padrao[linha][coluna])
                    definir_led(idx, 0, 0, nivel);
                else
                    definir_led(idx, 0, 0, 0);
                idx++;
            }
        } else {
            for (int coluna = 0; coluna < 5; coluna++) {
                if (padrao[linha][coluna])
                    definir_led(idx, 0, 0, nivel);
                else
                    definir_led(idx, 0, 0, 0);
                idx++;
            }
        }
    }
}
