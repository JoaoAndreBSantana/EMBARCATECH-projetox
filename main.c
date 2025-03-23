#include <stdio.h>
#include "neopixel.c"
#include "microphone_dma.c"
#include "buzzer.c"
#include "display_oled.c"
#include "buttonA.c"
#include "pwm.c"

#define MODO1 1
#define MODO2 2



int led_modo= MODO1;

void display_initial_message(uint8_t* ssd, struct render_area* frame_area) {
    char *text[] = {"   SOM  ", " DETECTADO"};
    int y = 30;
    for (uint i = 0; i < 2; i++) {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }
    ssd1306_draw_line(ssd, 0, 17, 128, 17, true);
    render_on_display(ssd, frame_area);
    
}
//função para alternar modos
void modo_led(){
  if (gpio_get(BUTTON_A) == 0) { // Botão pressionado
    // Alterna entre os modos 1 e 2
    led_modo = (led_modo == MODO1) ? MODO2 : MODO1;
    sleep_ms(200); // Debounce do botão
    }
}

void exibir_modo(uint8_t* ssd, struct render_area* frame_area) {
  memset(ssd, 0, ssd1306_buffer_length); // Limpa o display

  // Define a mensagem com base no modo
  char *modo_texto = (led_modo == MODO1) ? "  MODO 1  " : "  MODO 2  ";

  // Exibe o modo no topo do display
  ssd1306_draw_string(ssd, 5, 0, modo_texto);
  ssd1306_draw_line(ssd, 0, 17, 128, 17, true);
  
  render_on_display(ssd, frame_area);
}


int main() {

  stdio_init_all();  
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);

    gpio_pull_up(BUTTON_A);

  // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);


    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();
    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };
   calculate_render_area_buffer_length(&frame_area);
   // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    
        

  //parte dos leds
  npInit(LED_PIN, LED_COUNT);
  npClear();
  npWrite();

  // Preparação do ADC.
  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
    true, // Habilitar FIFO
    true, // Habilitar request de dados do DMA
    1, // Threshold para ativar request DMA é 1 leitura do ADC
    false, // Não usar bit de erro
    false // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);


  // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);

  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false); // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true); // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)
  
  channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC


    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);
  
    while (true) {
      modo_led();
      exibir_modo(ssd, &frame_area); 
        
      // Capta dados do microfone:
      float sound_level = mic_power(); // Usa a leitura direta do ADC
  
      // Compara o nível de som com um limiar fixo
      if (sound_level > 0.5f) {  // Limiar fixo (ajuste conforme necessário)
          display_initial_message(ssd, &frame_area);
          
          // Chama a função do modo correspondente
          if (led_modo == MODO1) {
              modo1();
          } else if (led_modo == MODO2) {
              modo2();
          }
  
          // Limpa o display e exibe o modo atual
          memset(ssd, 0, ssd1306_buffer_length);
          exibir_modo(ssd, &frame_area);
      }
  }
}
