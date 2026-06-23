# oled-acelerometro

Forma 3D (cubo ou triângulo/tetraedro) rotacionada em tempo real num display OLED **SSD1306**, controlada pela inclinação de um acelerômetro **MMA8452Q**. A forma gira conforme você move a placa, com leitura suavizada por filtro passa-baixa para um movimento estável.

Projeto em C++ para **Arduino Uno**, usando [PlatformIO](https://platformio.org/).

## Hardware

| Componente        | Detalhe                                  |
|-------------------|------------------------------------------|
| Placa             | Arduino Uno (ATmega328P, 16 MHz)         |
| Display           | OLED SSD1306 128x64, I2C (`0x3C`)        |
| Acelerômetro      | SparkFun MMA8452Q, I2C (`0x1D` ou `0x1C`)|

### Ligações I2C

Ambos os dispositivos compartilham o mesmo barramento I2C:

| Sinal | Arduino Uno |
|-------|-------------|
| SDA   | A4          |
| SCL   | A5          |
| VCC   | 3.3V / 5V   |
| GND   | GND         |

> Use resistores de pull-up em SDA/SCL caso seus módulos não os tenham — a falta deles é causa comum de falhas intermitentes com dois dispositivos no barramento.

## Dependências

Gerenciadas automaticamente pelo PlatformIO ([platformio.ini](platformio.ini)):

- `sparkfun/SparkFun_MMA8452Q`
- `adafruit/Adafruit SSD1306` (puxa `Adafruit GFX` e `Adafruit BusIO`)

## Como compilar e gravar

```bash
# Compilar
pio run

# Gravar na placa
pio run --target upload

# Abrir o monitor serial (9600 baud)
pio device monitor
```

## Diagnóstico e testes

O projeto inclui suporte a debug pela porta serial (**9600 baud**) para diagnosticar problemas de conexão.

No boot, o firmware:
1. Roda um **scanner I2C** e lista os endereços encontrados no barramento.
2. Inicializa display e acelerômetro de forma **não-bloqueante** (não trava se um falhar).
3. Para o acelerômetro, tenta os dois endereços possíveis (`0x1D` e `0x1C`).
4. Mostra o status (OK/FALHOU) na própria tela.

### Menu serial

Digite no monitor serial para testar cada dispositivo individualmente:

| Tecla | Ação                                      |
|-------|-------------------------------------------|
| `s`   | Scan do barramento I2C                    |
| `d`   | Testar apenas o display                   |
| `a`   | Testar apenas o acelerômetro (10 leituras)|
| `r`   | Reinicializar os dispositivos             |
| `c`   | Animar o cubo (modo normal)               |
| `t`   | Animar o triângulo (tetraedro)            |
| `?`   | Mostrar o menu                            |

Para desabilitar as mensagens de debug em produção, ajuste em [platformio.ini](platformio.ini):

```ini
build_flags =
    -D DEBUG_ENABLED=0
```

## Estrutura

```
src/
  main.cpp     # setup/loop: orquestra inicialização e modos
  config.h     # constantes de hardware (telas, endereços I2C)
  devices.*    # display, acelerômetro, scan I2C e leitura suavizada
  shapes.*     # formas 3D (cubo, triângulo) e renderização wireframe
  menu.*       # menu serial, testes e modo atual
  debug.h      # macros de debug controladas por DEBUG_ENABLED
platformio.ini
```

As formas são descritas apenas por dados (vértices + arestas) em `shapes.cpp`,
então adicionar uma nova forma não exige duplicar a lógica de rotação/projeção.

## Licença

Distribuído sob a licença MIT. Veja [LICENSE](LICENSE) para mais detalhes.
