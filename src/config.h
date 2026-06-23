#ifndef CONFIG_H
#define CONFIG_H

// ===========================================================================
//  Configuracao do hardware
// ===========================================================================

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1   // sem pino de reset dedicado

// Enderecos I2C conhecidos -------------------------------------------------
// O display SSD1306 normalmente e 0x3C (alguns modulos usam 0x3D).
// O MMA8452Q pode ser 0x1D (SA0=1, padrao) ou 0x1C (SA0=0), dependendo
// da placa. Por isso tentamos os dois na inicializacao.
#define OLED_ADDRESS       0x3C
#define ACCEL_ADDRESS_A    0x1D
#define ACCEL_ADDRESS_B    0x1C

#endif // CONFIG_H
