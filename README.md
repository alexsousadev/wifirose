# WiFi Rose

Este projeto implementa um sistema para Raspberry Pi Pico W que lê a posição de um joystick da placa Bitdoglab e transmite essas informações via WiFi. O sistema não apenas envia as coordenadas X e Y do joystick, mas também interpreta essas posições, indicando direções como Norte, Sul, Leste, Oeste, Nordeste etc.

## Funcionalidades

- Leitura das posições X e Y do joystick
- Conexão WiFi para transmissão dos dados
- Interpretação das posições em uma rosa dos ventos
- Interface web para visualização em tempo real

## Configuração do WiFi

Para configurar as credenciais do WiFi, edite o arquivo `CMakeLists.txt`. Localize as seguintes linhas no início do arquivo:

```cmake
# WiFi credentials
set(WIFI_SSID "Nome da Rede")
set(WIFI_PASSWORD "Senha da Rede")
```

Substitua:
- `Nome da Rede` pelo nome da sua rede WiFi (Ex.: "Multilaser 2.4")
- `Senha da Rede` pela senha da sua rede WiFi (Ex.: "12345678@")


## Estrutura do Projeto

O projeto está organizado em módulos específicos:
- `src/wifi_connect/`: Gerenciamento da conexão WiFi
- `src/tcp_utils/`: Utilitários para comunicação TCP
- `src/joystick/`: Leitura e processamento dos dados do joystick
- `src/send_joystick/`: Transmissão dos dados do joystick
- `src/server_html/`: Interface web para visualização

## Como Usar

1. Compile e envie o executável (`.uf2`) para o Pico W / BitDogLab
    > Caso seja apenas o Pico W, será necessário conectar os periféricos usados (Joystick e Botão)
2. Acesse a interface web através do IP do dispositivo que será exibido na interface serial (terminal)
5. Mova o joystick para ver as posições e direções atualizadas