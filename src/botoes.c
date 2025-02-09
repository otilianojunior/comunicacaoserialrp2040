#include "botoes.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "ssd1306.h"
#include "leds.h"

// Variáveis globais para debounce e estados dos botões
volatile uint32_t tempo_ultimo_interrupcao = 0;
volatile bool led_verde_aceso = false;
volatile bool led_azul_aceso = false;
volatile int id_numero = -1;

// Declarações externas de funções e variáveis de outros módulos
extern void desenhar_texto(tela_t *tela, const char *texto, uint8_t x, uint8_t y);
extern void enviar_dados_tela(tela_t *tela);
extern void definir_led_por_padrao(const uint8_t padrao[5][5]);
extern void escrever_leds(void);
extern const uint8_t padrao_numeros_led[10][5][5];
extern tela_t tela;
extern const uint PIN_LED_VERDE;
extern const uint PIN_LED_AZUL;

void callback_botao(uint gpio, uint32_t eventos) {
    uint32_t agora = to_ms_since_boot(get_absolute_time());
    if (agora - tempo_ultimo_interrupcao > ATRASO_DEBOUNCE_MS) {
        tempo_ultimo_interrupcao = agora;
        if (gpio == BOTAO_A_PIN) {
            led_verde_aceso = !led_verde_aceso;
            gpio_put(PIN_LED_VERDE, led_verde_aceso);
            desenhar_texto(&tela, led_verde_aceso ? "V ON " : "V OFF", 8, 48);
            printf(led_verde_aceso ? "LED Verde ON\n" : "LED Verde OFF\n");
        } else if (gpio == BOTAO_B_PIN) {
            led_azul_aceso = !led_azul_aceso;
            gpio_put(PIN_LED_AZUL, led_azul_aceso);
            desenhar_texto(&tela, led_azul_aceso ? "A ON " : "A OFF", 80, 48);
            printf(led_azul_aceso ? "LED Azul ON\n" : "LED Azul OFF\n");
        }
        // Se um número (0-9) estiver selecionado, atualiza os LEDs com o padrão correspondente
        if (id_numero >= 0 && id_numero <= 9) {
            definir_led_por_padrao(padrao_numeros_led[id_numero]);
            escrever_leds();
        }
        enviar_dados_tela(&tela);
    }
}

void inicializar_botoes(void) {
    gpio_init(BOTAO_A_PIN);
    gpio_init(BOTAO_B_PIN);
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);
    gpio_pull_up(BOTAO_A_PIN);
    gpio_pull_up(BOTAO_B_PIN);
    gpio_set_irq_enabled_with_callback(BOTAO_A_PIN, GPIO_IRQ_EDGE_FALL, true, callback_botao);
    gpio_set_irq_enabled_with_callback(BOTAO_B_PIN, GPIO_IRQ_EDGE_FALL, true, callback_botao);
}
