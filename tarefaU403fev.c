// Bibliotecas utilizadas
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h" //Display 
#include "font.h" //Disponibilizada pelo professor wilton
#include "hardware/pio.h" 
#include "ws2818b.pio.h"//Para a matriz de led

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define ENDERECO 0x3C

// Definições dos pinos do LED RGB
#define BLUE 12
#define GREEN 11
#define RED 13

//Inicializar os pinos do led RGB
void iniciar_rgb() {
  gpio_init(RED);
  gpio_init(GREEN);
  gpio_init(BLUE);

  gpio_set_dir(RED, GPIO_OUT);
  gpio_set_dir(GREEN, GPIO_OUT);
  gpio_set_dir(BLUE, GPIO_OUT);
}

// Função para ligar os leds com uma linha de código
void state(bool rr, bool gg, bool bb) {
 iniciar_rgb();
 gpio_put(RED, rr);
 gpio_put(GREEN, gg);
 gpio_put(BLUE, bb);
} 

ssd1306_t ssd;  // Declara a variável ssd de forma global
// Inicialização do I2C e do display OLED
void display() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Configurações dos botões
#define DEBOUNCE_DELAY 300  // Milissegundos
#define BOTAO_A 5  // Pino do Botão A
#define BOTAO_B 6  // Pino do Botão B

volatile uint32_t last_irq_time_A = 0;
volatile uint32_t last_irq_time_B = 0;

bool estado_LED_A = false;  // Estado do LED vermelho
bool estado_LED_B = false;  // Estado do LED verde

void botao_callback(uint gpio, uint32_t eventos);

void iniciar_botoes(void) {
    gpio_init(BOTAO_A);
    gpio_init(BOTAO_B);

    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_set_dir(BOTAO_B, GPIO_IN);

    gpio_pull_up(BOTAO_A);
    gpio_pull_up(BOTAO_B);

    // Configura as interrupções com callback para os botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, botao_callback);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, botao_callback);
}
//INTERRUPÇÃO E DEBOUNCE
void debounce_botao(uint pino, volatile uint32_t *last_irq_time, bool *estado_LED) {
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - *last_irq_time > DEBOUNCE_DELAY) {
        *last_irq_time = tempo_atual;

        // Alterna o estado do LED
        *estado_LED = !(*estado_LED);

        if (pino == BOTAO_A) {
          
            bool cor = true;
            ssd1306_fill(&ssd, !cor); // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
            ssd1306_draw_string(&ssd, "Acionando o", 20, 10); // Desenha uma string     
            ssd1306_draw_string(&ssd, "Led Vermelho", 20, 30); // Desenha uma string
            ssd1306_send_data(&ssd); // Atualiza o display
            state(*estado_LED, 0, 0);  // Alterna LED vermelho

        } else if (pino == BOTAO_B) {
            bool cor = true;
            ssd1306_fill(&ssd, !cor); // Limpa o display
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
            ssd1306_draw_string(&ssd, "Acionando o", 20, 10); // Desenha uma string     
            ssd1306_draw_string(&ssd, "Led Verde", 20, 30); // Desenha uma string     
            ssd1306_send_data(&ssd); // Atualiza o display
            state(0, *estado_LED, 0);  // Alterna LED verde
}}}

void botao_callback(uint gpio, uint32_t eventos) {
    if (gpio == BOTAO_A) {
        debounce_botao(BOTAO_A, &last_irq_time_A, &estado_LED_A);
    } else if (gpio == BOTAO_B) {
        debounce_botao(BOTAO_B, &last_irq_time_B, &estado_LED_B);
}}

#define PINO_MATRIZ 7  // Pino de controle da matriz de LEDs
#define NUM_LEDS 25    // Número total de LEDs na matriz
// Função para converter as posições (x, y) da matriz para um índice do vetor de LEDs
int getIndex(int x, int y) {
    if (x % 2 == 0) {
        return 24 - (x* 5 + y);
    } else {
        return 24 - (x * 5 + (4 - y));
}}

//Controlando o brilho para 30%
uint8_t BRILHO = 150;

// Definição da estrutura de cor para cada LED
struct pixel_t {
    uint8_t R, G, B;
};
typedef struct pixel_t npLED_t;
npLED_t leds[NUM_LEDS];

// PIO e state machine para controle dos LEDs
PIO np_pio;
uint sm;

// Função para atualizar os LEDs da matriz
void bf() {
    for (uint i = 0; i < NUM_LEDS; ++i) {
        pio_sm_put_blocking(np_pio, sm, leds[i].R);
        pio_sm_put_blocking(np_pio, sm, leds[i].G);
        pio_sm_put_blocking(np_pio, sm, leds[i].B); }
    sleep_us(100);
}
// Função de controle inicial da matriz de LEDs
void controle(uint pino) {
    uint offset = pio_add_program(pio0, &ws2818b_program);
    np_pio = pio0;
    sm = pio_claim_unused_sm(np_pio, true);
    ws2818b_program_init(np_pio, sm, offset, pino, 800000.f);

    for (uint i = 0; i < NUM_LEDS; ++i) {
        leds[i].R = leds[i].G = leds[i].B = 0;
    }
    bf();
}
// Função para configurar a cor de um LED específico
void cor(const uint indice, const uint8_t r, const uint8_t g, const uint8_t b) {
    leds[indice].R = r;
    leds[indice].G = g;
    leds[indice].B = b;
}
void desliga() {
    for (uint i = 0; i < NUM_LEDS; ++i) {
        cor(i, 0, 0, 0);
    }
    bf();
}

// DESENHAR OS NÚMEROS NA MATRIZ DE LEDS
void number0() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0},{0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  { 0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {0, 0, 0}, {BRILHO, 0, 0},  {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0}, {BRILHO, 0, 0},{0, 0, 0}}
    };
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number1() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, BRILHO, 10}, {0, 0, 0}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number2() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number3() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0, 0  }},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {0, 0, 0},     {0, 0,     0}, {100, BRILHO, 0}, {0, 0,   0}},
        {{0, 0, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {100, BRILHO, 0}, {0, 0,   0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number4() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a primeira matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number5() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, BRILHO, 0}, {0, 0, 0}}
    
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number6() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}, {0, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number7() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {0, BRILHO, BRILHO}, {0, BRILHO, BRILHO}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, BRILHO, BRILHO}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number8() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}, {BRILHO, 0, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {BRILHO, 0, BRILHO}, {0, 0, 0}}
    };

    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}
void number9() {
    int mat1[5][5][3] = {
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}},
        {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {BRILHO, BRILHO, BRILHO}, {0, 0, 0}}
    };


    // Exibir a matriz
    for (int linha = 0; linha < 5; linha++) {
        for (int cols = 0; cols < 5; cols++) {
            int posicao = getIndex(linha, cols);
            cor(posicao, mat1[linha][cols][0], mat1[linha][cols][1], mat1[linha][cols][2]);
        }
    }
    bf();
}

char atualizar = 0; // Estado atual do sistema
void comando(char atualizar) {
    switch (atualizar) {
        case '0': number0(); break;
        case '1': number1(); break;
        case '2': number2(); break;
        case '3': number3(); break;
        case '4': number4(); break;
        case '5': number5(); break;
        case '6': number6(); break;
        case '7': number7(); break;
        case '8': number8(); break;
        case '9': number9(); break;
        case 'r': case 'R' :  state(1,0,0); break;
        case 'g': case 'G' :  state(0,1,0); break;
        case 'b': case 'B' :  state(0,0,1); break;
        case 'y': case 'Y' :  state(1,1,0); break;
        case 'p': case 'P' :  state(1,0,1); break;
        case 'c': case 'C' :  state(0,1,1); break;
        case 'w': case 'W' :  state(1,1,1); break;
        default:  desliga();  state(0,0,0); break;
}}

int main() {
    stdio_init_all();
    controle(PINO_MATRIZ);
    // Inicialização dos botões com interrupções configuradas
    iniciar_rgb();     // Inicializa os LEDs RGB
    iniciar_botoes();  // Configura botões com interrupções
    display();

    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, "Utilize", 15, 10); // Desenha uma string
    ssd1306_draw_string(&ssd, "os botoes", 15, 30); // Desenha uma string
    ssd1306_draw_string(&ssd, "ou o teclado", 15, 50); // Desenha uma string      
    ssd1306_send_data(&ssd); // Atualiza o display

    bool cor = true;
    while (true) {
        cor = !cor;

      ssd1306_send_data(&ssd);
      printf("Iniciando...\n");
      sleep_ms(500);

        printf("Digite um caractere: ");
        if (scanf(" %c", &atualizar) == 1) {

            comando(atualizar);
            printf("%c\n", atualizar);
            ssd1306_fill(&ssd, !cor);
           ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);
           ssd1306_draw_string(&ssd, &atualizar, 63, 29);
          ssd1306_send_data(&ssd);}}
    return 0; 
}