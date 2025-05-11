# Tarefa U2C3 EmbarcaTeck: Sistema IoT

Este repositório apresenta um projeto de Internet das Coisas (IoT) utilizando a placa BitDogLab, centrada no microcontrolador Raspberry Pi Pico W. O objetivo é realizar a coleta e transmissão de dados oriundos de sensores digitais e analógicos, como botões, joystick e sensor DHT22, enviando-os para a plataforma ThingSpeak para monitoramento remoto através de uma interface web. O sistema é capaz de ler o estado de dois botões, a posição de um joystick, a umidade e temperatura do ambiente, e então envia esses dados pela internet, via protocolo HTTP. Essa implementação reforça o aprendizado prático de redes embarcadas e sensores no contexto da computação física.

## Objetivos do projeto

- Desenvolver a leitura de sensores digitais e analógicos na placa BitDogLab, através do microcontrolador Raspberry Pi Pico W.
- Explorar o uso de bibliotecas de rede e conexão Wi-Fi em C.
- Implementar comunicação de dados com serviços de nuvem utilizando HTTP.
- Mapear os eixos do joystick e convertê-los em direções cardeais (rosa dos ventos).
- Utilizar o servidor do ThingSpeak como plataforma de armazenamento e visualização remota.
- Desenvolver e exibir os dados de forma dinâmica em uma página web.


## Funcionalidades

- Comunicação via Wi-Fi.
- Coleta do status dos botões A e B da placa de desenvolvimento BitDogLab.
- Coleta dos dados dos eixos X e Y do joystick.
- Coleta dos dados do sensor de umidade e temperatura DHT22.
- Envio dos dados coletados para o servidor do ThingSpeak.


## Estrutura do Repositório
```
/
├── libs/            # Bibliotecas e sdk
├── comunicacao_iot_u2c3.c           # Arquivo C principal
├── CMakeList.txt    # Arquivo de compilação
└── README.md            # Este arquivo
```

## Configuração e Uso

### Compatibilidade
- Placa de desenvolvimento BitDogLab.
- Raspberry Pi Pico W.

### Compilação e Upload
1. Clone este repositório:
   ```
   https://github.com/PauloRLeal/comunicacao_iot_u2c3.git
   ```
2. Execute a extensão **Raspberry Pi Pico** do Visual Studio Code e abra o projeto na subpasta robo/
3. Atualize as linhas 11, 12 e 15 do código com as suas devidas credenciais:
   ```
   #define WIFI_SSID "your_ssid" 
   ```

   ```
   #define WIFI_PASSWORD "your_password" 
   ```

   ```
   #define API_KEY "your_thingspeak_api_key" 
   ```

4. Compile o código do sistema IoT para monitoramento remoto através da extensão
5. Faça o upload do firmware para o microcontrolador
6. Ligue o sistema IoT para monitoramento remoto e aguarde a conexão bem sucedida na rede WiFi. (Caso não conecte, reset o sistema)


## Possíveis dificuldades
Caso o seu sistema de controle e monitoramento não envie os dados para o servidor do ThingSpeak, verifique se configurou corretamente as informações da rede Wi-Fi e as credenciais do ThingSpeak.


## Direitos de Uso
Este projeto é disponibilizado para leitura e citação em outros trabalhos. Caso utilize este projeto como referência, por favor, forneça os devidos créditos ao autor.

## Autor
Desenvolvido por Paulo Roberto Araújo Leal.

## Anexos
- [Vídeo no youtube com explicação e o projeto em funcionamento](https://youtu.be/eVDgW0HW090?si=Aiii0x1b1JSktnU9)
- [Página web desenvolvida para visualização dos dados enviados ao servidor ThingSpeak](https://v0-real-time-web-app-nu.vercel.app/)
- [Visualização dos gráficos no servidor do ThingSpeak](https://thingspeak.mathworks.com/channels/2952597)
- [Documentação explicativa do projeto](https://docs.google.com/document/d/1QmjtPQbKv75fYgCW9ZC__xgsdDB3R5dtR7qd71a4LPk/edit?usp=sharing)
