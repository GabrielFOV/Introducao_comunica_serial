/*
 * Por: Wilton Lacerda Silva
 *    Comunicação serial com I2C
 *  
 * Uso da interface I2C para comunicação com o Display OLED
 * 
 * Estudo da biblioteca ssd1306 com PicoW na Placa BitDogLab.
 *  
 * Este programa escreve uma mensagem no display OLED.
 * 
 * 
*/


#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "auxiliar/ssd1306.h"
#include "auxiliar/font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define UART_ID uart0 // Seleciona a UART0
#define BAUD_RATE 115200 // Define a taxa de transmissão
#define UART_TX_PIN 0 // Pino GPIO usado para TX
#define UART_RX_PIN 1 // Pino GPIO usado para RX
#define led_pin_g 11
#define led_pin_b 12
#define led_pin_r 13

char apresentar[2];

int main()
{
  stdio_init_all();
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

    // Inicializa a UART
    uart_init(UART_ID, BAUD_RATE);

    // Configura os pinos GPIO para a UART
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Configura o pino 0 para TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Configura o pino 1 para RX

    // Mensagem inicial
    const char *init_message = "UART Demo - RP2\r\n"
                               "Digite algo e veja o eco:\r\n";
    uart_puts(UART_ID, init_message);

    gpio_init(led_pin_g);
    gpio_set_dir(led_pin_g, GPIO_OUT);
    gpio_init(led_pin_b);
    gpio_set_dir(led_pin_b, GPIO_OUT);
    gpio_init(led_pin_r);
    gpio_set_dir(led_pin_r, GPIO_OUT);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  bool cor = true;
  while (true)
  {
    cor = !cor;
    if (uart_is_readable(UART_ID)) 
    {
      // Lê um caractere da UART
      char caracter = uart_getc(UART_ID);
      apresentar[0] = caracter; 
      apresentar[1] = '\0'; 
      // Envia de volta o caractere lido (eco)
      uart_putc(UART_ID, caracter);

      // Envia uma mensagem adicional para cada caractere recebido
      uart_puts(UART_ID, " <- Eco do RP2\r\n");
    }
    //printf("%c", apresentar);
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
    ssd1306_draw_string(&ssd, apresentar, 60, 30); // Desenha uma string
   
    ssd1306_send_data(&ssd); // Atualiza o display

    sleep_ms(1000);
  }
}