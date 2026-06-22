# oled-acelerometro

Cubo 3D rotacionado em tempo real num display OLED **SSD1306**, controlado pela inclinação de um acelerômetro **MMA8452Q**. O cubo gira conforme você move a placa.

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
- `adafruit/Adafruit SSD1306`
- `elc0mpa/OLED_SSD1306_Chart`

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
| `c`   | Rodar a animação do cubo (modo normal)    |
| `?`   | Mostrar o menu                            |

Para desabilitar as mensagens de debug em produção, ajuste em [platformio.ini](platformio.ini):

```ini
build_flags =
    -D DEBUG_ENABLED=0
```

## Estrutura

```
src/
  main.cpp    # Lógica principal, diagnóstico e animação do cubo
  debug.h     # Macros de debug controladas por DEBUG_ENABLED
platformio.ini
```

## Licença

Sem licença definida.
