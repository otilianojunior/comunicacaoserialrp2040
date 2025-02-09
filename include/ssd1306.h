#ifndef SSD1306_H
#define SSD1306_H 

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Dimensões do display
#define LARGURA 128
#define ALTURA 64

// Comandos do display SSD1306 (valores fixos)
typedef enum {
   SET_CONTRAST = 0x81,
   SET_ENTIRE_ON = 0xA4,
   SET_NORM_INV = 0xA6,
   SET_DISP = 0xAE,
   SET_MEM_ADDR = 0x20,
   SET_COL_ADDR = 0x21,
   SET_PAGE_ADDR = 0x22,
   SET_DISP_START_LINE = 0x40,
   SET_SEG_REMAP = 0xA0,
   SET_MUX_RATIO = 0xA8,
   SET_COM_OUT_DIR = 0xC0,
   SET_DISP_OFFSET = 0xD3,
   SET_COM_PIN_CFG = 0xDA,
   SET_DISP_CLK_DIV = 0xD5,
   SET_PRECHARGE = 0xD9,
   SET_VCOM_DESEL = 0xDB,
   SET_CHARGE_PUMP = 0x8D
} ssd1306_command_t;

// Estrutura do display
typedef struct {
   uint8_t largura, altura, paginas, endereco;
   i2c_inst_t *porta_i2c;
   bool vcc_externo;
   uint8_t *buffer_ram;
   size_t tam_buffer;
   uint8_t buffer_porta[2];
} tela_t;

// Protótipos das funções do display
void inicializar_tela(tela_t *tela, uint8_t largura, uint8_t altura, bool vcc_externo, uint8_t endereco, i2c_inst_t *porta_i2c);
void configurar_tela(tela_t *tela);
void enviar_comando_tela(tela_t *tela, uint8_t comando);
void enviar_dados_tela(tela_t *tela);

void desenhar_pixel(tela_t *tela, uint8_t x, uint8_t y, bool valor);
void preencher_tela(tela_t *tela, bool valor);
void desenhar_retangulo(tela_t *tela, uint8_t topo, uint8_t esquerda, uint8_t largura, uint8_t altura, bool valor, bool preencher);
void desenhar_linha(tela_t *tela, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool valor);
void desenhar_linha_horizontal(tela_t *tela, uint8_t x0, uint8_t x1, uint8_t y, bool valor);
void desenhar_linha_vertical(tela_t *tela, uint8_t x, uint8_t y0, uint8_t y1, bool valor);
void desenhar_caractere(tela_t *tela, char c, uint8_t x, uint8_t y);
void desenhar_texto(tela_t *tela, const char *texto, uint8_t x, uint8_t y);

#endif
