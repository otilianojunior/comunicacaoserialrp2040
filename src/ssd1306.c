#include "ssd1306.h"
#include "font.h"

// Inicializa o display
void inicializar_tela(tela_t *tela, uint8_t largura, uint8_t altura, bool vcc_externo, uint8_t endereco, i2c_inst_t *porta_i2c) {
    tela->largura = largura;
    tela->altura = altura;
    tela->paginas = altura / 8U;
    tela->endereco = endereco;
    tela->porta_i2c = porta_i2c;
    tela->tam_buffer = tela->paginas * tela->largura + 1;
    tela->buffer_ram = calloc(tela->tam_buffer, sizeof(uint8_t));
    tela->buffer_ram[0] = 0x40;
    tela->buffer_porta[0] = 0x80;
}

// Configura o display com os comandos necessários
void configurar_tela(tela_t *tela) {
    enviar_comando_tela(tela, SET_DISP | 0x00);
    enviar_comando_tela(tela, SET_MEM_ADDR);
    enviar_comando_tela(tela, 0x01);
    enviar_comando_tela(tela, SET_DISP_START_LINE | 0x00);
    enviar_comando_tela(tela, SET_SEG_REMAP | 0x01);
    enviar_comando_tela(tela, SET_MUX_RATIO);
    enviar_comando_tela(tela, tela->altura - 1);
    enviar_comando_tela(tela, SET_COM_OUT_DIR | 0x08);
    enviar_comando_tela(tela, SET_DISP_OFFSET);
    enviar_comando_tela(tela, 0x00);
    enviar_comando_tela(tela, SET_COM_PIN_CFG);
    enviar_comando_tela(tela, 0x12);
    enviar_comando_tela(tela, SET_DISP_CLK_DIV);
    enviar_comando_tela(tela, 0x80);
    enviar_comando_tela(tela, SET_PRECHARGE);
    enviar_comando_tela(tela, 0xF1);
    enviar_comando_tela(tela, SET_VCOM_DESEL);
    enviar_comando_tela(tela, 0x30);
    enviar_comando_tela(tela, SET_CONTRAST);
    enviar_comando_tela(tela, 0xFF);
    enviar_comando_tela(tela, SET_ENTIRE_ON);
    enviar_comando_tela(tela, SET_NORM_INV);
    enviar_comando_tela(tela, SET_CHARGE_PUMP);
    enviar_comando_tela(tela, 0x14);
    enviar_comando_tela(tela, SET_DISP | 0x01);
}

// Envia um comando ao display
void enviar_comando_tela(tela_t *tela, uint8_t comando) {
    tela->buffer_porta[1] = comando;
    i2c_write_blocking(tela->porta_i2c, tela->endereco, tela->buffer_porta, 2, false);
}

// Envia os dados do buffer para o display
void enviar_dados_tela(tela_t *tela) {
    enviar_comando_tela(tela, SET_COL_ADDR);
    enviar_comando_tela(tela, 0);
    enviar_comando_tela(tela, tela->largura - 1);
    enviar_comando_tela(tela, SET_PAGE_ADDR);
    enviar_comando_tela(tela, 0);
    enviar_comando_tela(tela, tela->paginas - 1);
    i2c_write_blocking(tela->porta_i2c, tela->endereco, tela->buffer_ram, tela->tam_buffer, false);
}

// Desenha um pixel no display
void desenhar_pixel(tela_t *tela, uint8_t x, uint8_t y, bool valor) {
    uint16_t indice = (y >> 3) + (x << 3) + 1;
    uint8_t pixel = (y & 0b111);
    if (valor)
        tela->buffer_ram[indice] |= (1 << pixel);
    else
        tela->buffer_ram[indice] &= ~(1 << pixel);
}

// Preenche todo o display com um valor
void preencher_tela(tela_t *tela, bool valor) {
    for (uint8_t y = 0; y < tela->altura; ++y) {
        for (uint8_t x = 0; x < tela->largura; ++x) {
            desenhar_pixel(tela, x, y, valor);
        }
    }
}

// Desenha um retângulo (opcionalmente preenchido)
void desenhar_retangulo(tela_t *tela, uint8_t topo, uint8_t esquerda, uint8_t largura, uint8_t altura, bool valor, bool preenche) {
    for (uint8_t x = esquerda; x < esquerda + largura; ++x) {
        desenhar_pixel(tela, x, topo, valor);
        desenhar_pixel(tela, x, topo + altura - 1, valor);
    }
    for (uint8_t y = topo; y < topo + altura; ++y) {
        desenhar_pixel(tela, esquerda, y, valor);
        desenhar_pixel(tela, esquerda + largura - 1, y, valor);
    }
    if (preenche) {
        for (uint8_t x = esquerda + 1; x < esquerda + largura - 1; ++x) {
            for (uint8_t y = topo + 1; y < topo + altura - 1; ++y) {
                desenhar_pixel(tela, x, y, valor);
            }
        }
    }
}

// Desenha uma linha usando o algoritmo de Bresenham
void desenhar_linha(tela_t *tela, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool valor) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int erro = dx - dy;
    while (true) {
        desenhar_pixel(tela, x0, y0, valor);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = erro * 2;
        if (e2 > -dy) {
            erro -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            erro += dx;
            y0 += sy;
        }
    }
}

// Desenha uma linha horizontal
void desenhar_linha_horizontal(tela_t *tela, uint8_t x0, uint8_t x1, uint8_t y, bool valor) {
    for (uint8_t x = x0; x <= x1; ++x)
        desenhar_pixel(tela, x, y, valor);
}

// Desenha uma linha vertical
void desenhar_linha_vertical(tela_t *tela, uint8_t x, uint8_t y0, uint8_t y1, bool valor) {
    for (uint8_t y = y0; y <= y1; ++y)
        desenhar_pixel(tela, x, y, valor);
}

// Desenha um caractere (8x8) utilizando a fonte
void desenhar_caractere(tela_t *tela, char c, uint8_t x, uint8_t y) {
    uint16_t indice = 0;
    if (c >= 'A' && c <= 'Z')
        indice = (c - 'A' + 11) * 8;    // Letras maiúsculas
    else if (c >= '0' && c <= '9')
        indice = (c - '0' + 1) * 8;     // Dígitos
    else if (c >= 'a' && c <= 'z')
        indice = (c - 'a' + 37) * 8;    // Letras minúsculas
    else if (c >= '!' && c <= '/')
        indice = (c - '!' + 63) * 8;    // Caracteres especiais

    for (uint8_t i = 0; i < 8; ++i) {
        uint8_t linha = font[indice + i];
        for (uint8_t j = 0; j < 8; ++j) {
            desenhar_pixel(tela, x + i, y + j, linha & (1 << j));
        }
    }
}

// Desenha uma string no display
void desenhar_texto(tela_t *tela, const char *texto, uint8_t x, uint8_t y) {
    while (*texto) {
        desenhar_caractere(tela, *texto++, x, y);
        x += 8;
        if (x + 8 >= tela->largura) {
            x = 0;
            y += 8;
        }
        if (y + 8 >= tela->altura)
            break;
    }
}
