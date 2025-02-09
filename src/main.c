#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"

#include "ssd1306.h"
#include "font.h"
#include "ws2812.pio.h"

#include "leds.h"
#include "botoes.h"

// Pinos dos LEDs RGB
const uint PIN_LED_VERDE = 11;
const uint PIN_LED_AZUL   = 12;

// Configuração da comunicação I2C com o display
#define PORTA_I2C    i2c1
#define I2C_SDA      14
#define I2C_SCL      15
#define ENDERECO     0x3C

// Configuração do display SSD1306
#define LARGURA_TELA 128
#define ALTURA_TELA  64

// Parâmetros do "terminal"
// (MAX_LINHAS: número de linhas; MAX_COLUNAS: número máximo de caracteres por linha)
#define MAX_LINHAS   3
#define MAX_COLUNAS  16

// Buffer de texto para o terminal
char buffer_terminal[MAX_LINHAS][MAX_COLUNAS + 1];  // +1 para o caractere nulo
int linha_atual = 0;
int coluna_atual = 0;

// Variáveis globais para uso do PIO (necessárias pelo módulo de LEDs)
PIO pio;
uint sm;

// Estrutura do display
tela_t tela;

/*
 * atualizar_terminal:
 *   - Limpa o display,
 *   - Desenha cada linha do terminal (utilizando o buffer),
 *   - Exibe os status dos LEDs (verde e azul),
 *   - Envia os dados para o display.
 */
void atualizar_terminal(tela_t *tela) {
    preencher_tela(tela, false);
    for (int i = 0; i < MAX_LINHAS; i++) {
        int y = 10 + i * 9;
        desenhar_texto(tela, buffer_terminal[i], 8, y);
    }
    char status_verde[10];
    char status_azul[10];
    sprintf(status_verde, "V %s", gpio_get(PIN_LED_VERDE) ? "ON" : "OFF");
    sprintf(status_azul,  "A %s", gpio_get(PIN_LED_AZUL) ? "ON" : "OFF");
    desenhar_texto(tela, status_verde, 8, 48);
    desenhar_texto(tela, status_azul, 80, 48);
    enviar_dados_tela(tela);
}

/*
 * rolar_terminal:
 *   - Remove a primeira linha do buffer (fazendo "shift" para cima)
 *   - Limpa a última linha e reinicia os índices para nova escrita.
 */
void rolar_terminal(void) {
    for (int i = 0; i < MAX_LINHAS - 1; i++) {
        strcpy(buffer_terminal[i], buffer_terminal[i + 1]);
    }
    buffer_terminal[MAX_LINHAS - 1][0] = '\0';
    linha_atual = MAX_LINHAS - 1;
    coluna_atual = 0;
}

/*
 * inicializar_gpio:
 *   - Configura os pinos dos LEDs RGB e os pinos I2C para o display.
 */
void inicializar_gpio(void) {
    gpio_init(PIN_LED_VERDE);
    gpio_init(PIN_LED_AZUL);
    gpio_set_dir(PIN_LED_VERDE, GPIO_OUT);
    gpio_set_dir(PIN_LED_AZUL, GPIO_OUT);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

/*
 * inicializar_pio:
 *   - Configura o PIO para controlar a matriz de LEDs.
 */
void inicializar_pio(uint pino) {
    uint offset = pio_add_program(pio0, &ws2812_program);
    pio = pio0;
    sm = pio_claim_unused_sm(pio, false);
    ws2812_program_init(pio, sm, offset, pino, 800000, false);
}

/*
 * inicializar_tela_display:
 *   - Inicializa e configura o display SSD1306.
 *   - Limpa o display para prepará-lo como terminal.
 */
void inicializar_tela_display(void) {
    inicializar_tela(&tela, LARGURA_TELA, ALTURA_TELA, false, ENDERECO, PORTA_I2C);
    configurar_tela(&tela);
    preencher_tela(&tela, false);
    enviar_dados_tela(&tela);
}

int main(void) {
    stdio_init_all();
    sleep_ms(500);

    inicializar_gpio();
    // Inicializa o PIO para controlar a matriz de LEDs (o pino definido em leds.h)
    inicializar_pio(PIN_LED_MTX);
    limpar_leds();
    escrever_leds();
    i2c_init(PORTA_I2C, 400 * 1000);
    inicializar_tela_display();
    inicializar_botoes();  // Inicializa os botões e suas interrupções

    // Inicializa o buffer do terminal com a mensagem "Digite..."
    strcpy(buffer_terminal[0], "Digite...");
    linha_atual = 0;
    coluna_atual = strlen(buffer_terminal[0]);
    for (int i = 1; i < MAX_LINHAS; i++) {
        buffer_terminal[i][0] = '\0';
    }
    atualizar_terminal(&tela);

    char c;
    while (true) {
        sleep_ms(10);  // Atualiza a cada 10ms
        scanf("%c", &c);

        // Se o prompt "Digite..." ainda estiver, apaga-o
        if (linha_atual == 0 && strcmp(buffer_terminal[0], "Digite...") == 0) {
            buffer_terminal[0][0] = '\0';
            coluna_atual = 0;
        }

        // Se for ENTER (newline ou carriage return), pula para a próxima linha
        if (c == '\n' || c == '\r') {
            if (linha_atual == MAX_LINHAS - 1) {
                rolar_terminal();
            } else {
                linha_atual++;
            }
            coluna_atual = 0;
        }
        // Se for BACKSPACE (ASCII 8 ou 127), apaga o último caractere
        else if (c == 0x08 || c == 127) {
            if (coluna_atual > 0) {
                coluna_atual--;
                buffer_terminal[linha_atual][coluna_atual] = '\0';
            }
        }
        // Se for outro caractere
        else {
            // Se for um dígito, atualiza os LEDs (cor azul)
            if (c >= '0' && c <= '9') {
                int num = c - '0';
                definir_led_por_padrao(padrao_numeros_led[num]);
                escrever_leds();
            }
            buffer_terminal[linha_atual][coluna_atual] = c;
            coluna_atual++;
            buffer_terminal[linha_atual][coluna_atual] = '\0';  // Garante terminação nula

            // Se a linha estiver completa, pula para a próxima
            if (coluna_atual >= MAX_COLUNAS) {
                if (linha_atual == MAX_LINHAS - 1) {
                    rolar_terminal();
                } else {
                    linha_atual++;
                }
                coluna_atual = 0;
            }
        }
        atualizar_terminal(&tela);
    }
}
