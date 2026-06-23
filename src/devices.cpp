#include <Wire.h>
#include "devices.h"
#include "config.h"
#include "debug.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MMA8452Q accel;

bool displayOK = false;
bool accelOK   = false;
uint8_t accelAddress = 0;

void busBegin() {
  Wire.begin();
}

// ===========================================================================
//  Diagnostico do barramento I2C
// ===========================================================================
void scanI2C() {
  DBG_PRINTLN(F("--- Scanner I2C ---"));
  uint8_t count = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      count++;
      DBG_PRINT(F("  Dispositivo encontrado em 0x"));
      if (addr < 16) DBG_PRINT('0');
      DBG_PRINTLN(addr, HEX);
    }
  }
  if (count == 0) {
    DBG_PRINTLN(F("  Nenhum dispositivo encontrado!"));
    DBG_PRINTLN(F("  Verifique fiacao SDA/SCL, alimentacao e resistores pull-up."));
  } else {
    DBG_KV("  Total de dispositivos", count);
  }
  DBG_PRINTLN(F("-------------------"));
}

bool i2cPresent(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

// ===========================================================================
//  Inicializacao individual de cada dispositivo (nao trava se falhar)
// ===========================================================================
bool initDisplay() {
  DBG_PRINT(F("Inicializando display SSD1306 em 0x"));
  DBG_PRINTLN(OLED_ADDRESS, HEX);

  if (!i2cPresent(OLED_ADDRESS)) {
    DBG_PRINTLN(F("  ERRO: display nao responde no barramento I2C."));
    return false;
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    DBG_PRINTLN(F("  ERRO: display.begin() falhou."));
    return false;
  }
  display.clearDisplay();
  display.display();
  DBG_PRINTLN(F("  OK: display inicializado."));
  return true;
}

bool initAccel() {
  DBG_PRINTLN(F("Inicializando acelerometro MMA8452Q..."));

  // Tenta os dois enderecos possiveis (SA0 = 0 ou 1)
  uint8_t candidates[2] = { ACCEL_ADDRESS_A, ACCEL_ADDRESS_B };
  for (uint8_t i = 0; i < 2; i++) {
    uint8_t addr = candidates[i];
    DBG_PRINT(F("  Tentando 0x"));
    DBG_PRINTLN(addr, HEX);
    if (!i2cPresent(addr)) {
      DBG_PRINTLN(F("    nao responde."));
      continue;
    }
    if (accel.begin(Wire, addr)) {
      accelAddress = addr;
      DBG_PRINT(F("  OK: acelerometro inicializado em 0x"));
      DBG_PRINTLN(addr, HEX);
      return true;
    }
    DBG_PRINTLN(F("    respondeu mas begin() falhou (WHO_AM_I incorreto?)."));
  }
  DBG_PRINTLN(F("  ERRO: acelerometro nao encontrado."));
  return false;
}

// ===========================================================================
//  Leitura suavizada (filtro passa-baixa / media movel exponencial)
// ---------------------------------------------------------------------------
//  filtrado += ALPHA * (bruto - filtrado)
//  ALPHA menor  -> mais suave, porem responde mais devagar.
//  ALPHA maior  -> responde rapido, porem treme mais.
// ===========================================================================
static const float FILTER_ALPHA = 0.20f;
static float fx = 0, fy = 0, fz = 0;
static bool filterInit = false;

void accelResetFilter() {
  filterInit = false;
}

void accelReadSmoothed(float &x, float &y, float &z) {
  accel.read();
  float rx = accel.x, ry = accel.y, rz = accel.z;

  if (!filterInit) {
    // Primeira amostra: usa o valor bruto direto para nao "vir do zero".
    fx = rx; fy = ry; fz = rz;
    filterInit = true;
  } else {
    fx += FILTER_ALPHA * (rx - fx);
    fy += FILTER_ALPHA * (ry - fy);
    fz += FILTER_ALPHA * (rz - fz);
  }

  x = fx; y = fy; z = fz;
}
