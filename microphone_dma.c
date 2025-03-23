#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "neopixel.c"
#include "buzzer.c"


// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 600 // Aumentei o número de amostras para melhorar a detecção.
#define ADC_ADJUST(x) (x) // Removi o ajuste para Volts, usando o valor bruto do ADC.
// Pino e número de LEDs da matriz de LEDs.
#define LED_PIN 7
#define LED_COUNT 25

#define abs(x) ((x < 0) ? (-x) : (x))

// Canal e configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;

// Buffer de amostras do ADC.
uint16_t adc_buffer[SAMPLES];

void sample_mic();
float mic_power();
float calcular_limiar(); // Adicionei esta função para calcular o limiar dinâmico.

// Funções de modo (não alteradas)
void exibirMatriz(int matriz[5][5][3], int tempo_ms) {
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz[linha][coluna][0], matriz[linha][coluna][1], matriz[linha][coluna][2]);
        }
    }
    npWrite();
    alarm_sound(BUZZER_PIN, 3000); 
    sleep_ms(100);
}

void modo1() {
  // Matrizes para o efeito de fogo progressivo
  int matriz_coracao2[5][5][3] = { 
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
    {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
    {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}},        
    {{1, 0, 0}, {1, 0, 0}, {0, 0, 0}, {1, 0, 0}, {1, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
};

 int matriz_coracao[5][5][3] = { 
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
      {{0, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {0, 0, 0}},    
      {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}},        
      {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}},
      {{0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}}
  };

  int matriz_coracao3[5][5][3] = { 
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 1}, {0, 0, 0}, {0, 0, 0}},    
    {{0, 0, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 0}},    
    {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},        
    {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},
    {{0, 0, 0}, {0, 0, 1}, {0, 0, 0}, {0, 0, 1}, {0, 0, 0}}
};


  // Número de vezes que as matrizes piscarão
  int num_piscadas = 1; // Aumentei o número de piscadas para o efeito durar mais

  // Loop para piscar as matrizes
  for (int i = 0; i < num_piscadas; i++) {
      // Exibe as matrizes de fogo em sequência
      exibirMatriz(matriz_coracao2, 200); // Exibe por 200ms
      npClear(); // Limpa a matriz
      npWrite(); // Atualiza a exibição
      sleep_ms(50); // Aguarda 50ms

      exibirMatriz(matriz_coracao, 200); // Exibe por 200ms
      npClear(); // Limpa a matriz
      npWrite(); // Atualiza a exibição
      sleep_ms(50); // Aguarda 50ms

  }

  // Exibe a última matriz de fogo por mais tempo no final
  exibirMatriz(matriz_coracao3, 500); // Exibe por 500ms
  npClear();
  npWrite();
}




void modo2() {
  
   int matriz_seta_5[5][5][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}}, 
    {{0, 0, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 0, 0}},
    {{0, 1, 0}, {0, 1, 0}, {0, 0, 0}, {0, 1, 0}, {0, 1, 0}},
    {{0, 0, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}}
  };

int matriz_seta_6[5][5][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, 
    {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}
  };

  int matriz_seta_7[5][5][3] = {
    {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}}, 
    {{0, 0, 0}, {0, 1, 0}, {1, 1, 1}, {0, 1, 0}, {0, 0, 0}},
    {{0, 1, 0}, {1, 1, 1}, {1, 0, 0}, {1, 1, 1}, {0, 1, 0}},
    {{0, 0, 0}, {0, 1, 0}, {1, 1, 1}, {0, 1, 0}, {0, 0, 0}},
    {{0, 0, 0}, {0, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}}
  };


  // Número de vezes que as matrizes piscarão
  int num_piscadas = 1; // Aumentei o número de piscadas para o efeito durar mais

  // Loop para piscar as matrizes
  for (int i = 0; i < num_piscadas; i++) {
      // Exibe as matrizes de fogo em sequência
      exibirMatriz(matriz_seta_6, 200); // Exibe por 200ms
      npClear(); // Limpa a matriz
      npWrite(); // Atualiza a exibição
      sleep_ms(50); // Aguarda 50ms

      exibirMatriz(matriz_seta_5, 200); // Exibe por 200ms
      npClear(); // Limpa a matriz
      npWrite(); // Atualiza a exibição
      sleep_ms(50); // Aguarda 50ms

  }

  // Exibe a última matriz de fogo por mais tempo no final
  exibirMatriz(matriz_seta_7, 500); // Exibe por 500ms
  npClear();
  npWrite();
}

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic() {
    adc_fifo_drain(); // Limpa o FIFO do ADC.
    adc_run(false); // Desliga o ADC (se estiver ligado) para configurar o DMA.

    dma_channel_configure(dma_channel, &dma_cfg,
        adc_buffer, // Escreve no buffer.
        &(adc_hw->fifo), // Lê do ADC.
        SAMPLES, // Faz SAMPLES amostras.
        true // Liga o DMA.
    );

    // Liga o ADC e espera acabar a leitura.
    adc_run(true);
    dma_channel_wait_for_finish_blocking(dma_channel);

    // Acabou a leitura, desliga o ADC de novo.
    adc_run(false);
}

/**
 * Calcula a potência máxima das leituras do ADC.
 * Substituí o RMS pelo valor máximo para detectar melhor sons impulsivos.
 */
float mic_power() {
  uint16_t raw_adc = adc_read(); // Lê o valor bruto do ADC
  float voltage = (raw_adc * 3.3f) / 4095; // Converte para tensão (0-3.3V)
  float sound_level = fabs(voltage - 1.65f); // Subtrai o offset (1.65V)
  return sound_level; // Retorna o nível de som
}

/**
 * Função para calcular o limiar dinâmico com base no ruído de fundo.
 */
float calcular_limiar() {
    float soma = 0;
    for (uint i = 0; i < SAMPLES; ++i) {
        soma += adc_buffer[i];
    }
    float media = soma / SAMPLES;
    return media * 2.0f;  // Limiar é 2x a média do ruído
}