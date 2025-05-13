# 🎮 EMBARCATECH - Sistema Interativo de Resposta Audiovisual com Matriz de LEDs

**Link do GitHub**: [EMBARCATECH - Projeto GitHub](https://github.com/JoaoAndreBSantana/EMBARCATECH-projetox.git)  
**Vídeo no YouTube**: [Assista no YouTube](https://youtu.be/RhOCiRXP3es?si=ava4k8WGKjnl4qoW)

## 📜 Descrição

Este projeto é um sistema embarcado desenvolvido para controlar LEDs e monitorar o nível de som com base na leitura de um microfone. O sistema alterna entre dois modos de operação, utilizando um display OLED para exibir informações sobre o status do sistema e o modo atual. Além disso, é possível ouvir um som com o buzzer quando um nível de som é detectado.

## 🚀 Funcionalidades

- **Controle de LEDs**: O sistema pode alternar entre dois modos de operação. A alternância é realizada através de um botão.
- **Monitoramento de Som**: O sistema monitora o nível de som captado por um microfone. Se o som ultrapassar um limiar, uma mensagem é exibida no display OLED.
- **Display OLED**: Exibe informações sobre o sistema, como o modo atual e a mensagem "SOM DETECTADO" quando o nível de som é alto.
- **Buzzer**: Um buzzer é acionado quando o nível de som é detectado.
- **Modo de Operação**: O sistema alterna entre dois modos de operação, com configurações de LEDs diferentes.

## 📂 Estrutura de Arquivos

- `neopixel.c`: Implementação de controle de LEDs.
- `microphone_dma.c`: Leitura do microfone utilizando DMA.
- `buzzer.c`: Controle do buzzer.
- `display_oled.c`: Manipulação do display OLED.
- `buttonA.c`: Funções relacionadas ao botão.
- `pwm.c`: Controle de PWM para o buzzer.

## 🛠️ Dependências

Este projeto depende das seguintes bibliotecas e configurações de hardware:

- **Microcontrolador**: Utiliza um microcontrolador com suporte para GPIO, I2C, ADC, e PWM.
- **Display OLED SSD1306**: Utiliza o protocolo I2C para comunicação.
- **Neopixel**: Utiliza LEDs endereçáveis para controle de cores.
- **Microfone**: A leitura do microfone é feita via ADC e DMA.
- **Buzzer**: Controlado via PWM.

## ⚙️ Configuração

### 📌 Pinos

- **BUTTON_A**: Pino do botão para alternar entre os modos de operação.
- **LED_PIN**: Pino para controlar os LEDs.
- **MIC_PIN**: Pino para o microfone.
- **BUZZER_PIN**: Pino para o buzzer.

### 🖥️ Inicialização do I2C

A comunicação com o display OLED é feita através do I2C, com uma taxa de 100kHz.

### 🔊 Configuração do ADC

O microfone é lido via ADC, e o DMA é utilizado para transferir os dados de forma eficiente.

### 🎶 Inicialização do PWM

O PWM é configurado para o buzzer para emitir sons quando o nível de som é detectado.

