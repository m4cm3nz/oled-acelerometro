#ifndef DEVICES_H
#define DEVICES_H

#include <Adafruit_SSD1306.h>
#include <SparkFun_MMA8452Q.h>

// ===========================================================================
//  Dispositivos do barramento I2C: display, acelerometro e seu estado.
// ---------------------------------------------------------------------------
//  Os objetos e flags sao globais (compartilhados entre os modulos), mas o
//  acesso fica concentrado aqui para manter o resto do codigo desacoplado.
// ===========================================================================

extern Adafruit_SSD1306 display;
extern MMA8452Q accel;

// Estado dos dispositivos: permite o programa continuar mesmo se um falhar,
// em vez de travar para sempre.
extern bool displayOK;
extern bool accelOK;
extern uint8_t accelAddress;

// Inicia o barramento I2C (Wire.begin()).
void busBegin();

// Diagnostico: varre o barramento e lista os enderecos que respondem.
void scanI2C();

// Verifica se um endereco responde no barramento.
bool i2cPresent(uint8_t addr);

// Inicializacao individual (nao trava se falhar; devolve true em caso de OK).
bool initDisplay();
bool initAccel();

// Leitura do acelerometro com filtro passa-baixa (media movel exponencial),
// deixando o movimento das formas suave em vez de tremido. Devolve os valores
// filtrados (na mesma escala bruta de accel.x/y/z).
void accelReadSmoothed(float &x, float &y, float &z);

// Descarta o estado do filtro para que a proxima leitura recomece "limpa".
void accelResetFilter();

// Detecta uma "chacoalhada": pico na magnitude da aceleracao acima de um
// limiar. Como a gravidade mantem ~1g em qualquer inclinacao estatica, apenas
// um movimento brusco dispara (inclinar a placa para girar a forma nao conta).
// Usa a ultima amostra lida por accelReadSmoothed(), entao chame logo depois.
// Tem cooldown interno para nao disparar varias vezes na mesma chacoalhada.
bool accelShakeDetected();

#endif // DEVICES_H
