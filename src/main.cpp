#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_MMA8452Q.h>
#include <math.h>
#include "debug.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Enderecos I2C conhecidos -------------------------------------------------
// O display SSD1306 normalmente e 0x3C (alguns modulos usam 0x3D).
// O MMA8452Q pode ser 0x1D (SA0=1, padrao) ou 0x1C (SA0=0), dependendo
// da placa. Por isso tentamos os dois na inicializacao.
#define OLED_ADDRESS       0x3C
#define ACCEL_ADDRESS_A    0x1D
#define ACCEL_ADDRESS_B    0x1C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MMA8452Q accel;

// Estado dos dispositivos: permite o programa continuar mesmo se um falhar,
// em vez de travar para sempre (problema da versao anterior).
bool displayOK = false;
bool accelOK   = false;
uint8_t accelAddress = 0;

// Coordenadas 3D do cubo
float cubeVertices[8][3] = {
  {1, 1, 1}, {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1},   // Frente
  {1, 1, -1}, {-1, 1, -1}, {-1, -1, -1}, {1, -1, -1} // Tras
};

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

// Verifica se um endereco responde no barramento
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
//  Rotinas de teste individuais (acionadas pelo menu serial)
// ===========================================================================
void testDisplay() {
  DBG_PRINTLN(F("[TESTE] Display"));
  if (!displayOK) {
    DBG_PRINTLN(F("  Display nao inicializado. Tentando novamente..."));
    displayOK = initDisplay();
    if (!displayOK) return;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Teste do Display"));
  display.println(F("SSD1306 OK"));
  display.display();

  // Padroes visuais para verificar pixels
  delay(800);
  display.invertDisplay(true);
  delay(400);
  display.invertDisplay(false);
  display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
  DBG_PRINTLN(F("  OK: se voce ve texto e uma borda na tela, o display funciona."));
}

void testAccel() {
  DBG_PRINTLN(F("[TESTE] Acelerometro"));
  if (!accelOK) {
    DBG_PRINTLN(F("  Acelerometro nao inicializado. Tentando novamente..."));
    accelOK = initAccel();
    if (!accelOK) return;
  }
  DBG_PRINTLN(F("  Lendo 10 amostras (x, y, z em g):"));
  for (uint8_t i = 0; i < 10; i++) {
    if (accel.available()) {
      accel.read();
      DBG_PRINT(F("    x="));
      DBG_PRINT(accel.cx, 3);
      DBG_PRINT(F("  y="));
      DBG_PRINT(accel.cy, 3);
      DBG_PRINT(F("  z="));
      DBG_PRINT(accel.cz, 3);
      DBG_PRINT(F("   (raw "));
      DBG_PRINT(accel.x); DBG_PRINT(F(","));
      DBG_PRINT(accel.y); DBG_PRINT(F(","));
      DBG_PRINT(accel.z); DBG_PRINTLN(F(")"));
    } else {
      DBG_PRINTLN(F("    sem dados novos..."));
    }
    delay(150);
  }
  DBG_PRINTLN(F("  OK: valores devem mudar quando voce inclina a placa."));
}

void printMenu() {
  DBG_PRINTLN(F(""));
  DBG_PRINTLN(F("===== MENU DE DIAGNOSTICO ====="));
  DBG_PRINTLN(F("  s = scan do barramento I2C"));
  DBG_PRINTLN(F("  d = testar display"));
  DBG_PRINTLN(F("  a = testar acelerometro"));
  DBG_PRINTLN(F("  r = reinicializar dispositivos"));
  DBG_PRINTLN(F("  c = rodar animacao do cubo (modo normal)"));
  DBG_PRINTLN(F("  ? = mostrar este menu"));
  DBG_PRINTLN(F("==============================="));
}

// ===========================================================================
//  Animacao do cubo (modo normal)
// ===========================================================================
void drawCube(float angleX, float angleY, float angleZ) {
  int scale = 60;
  int xCenter = SCREEN_WIDTH / 2;
  int yCenter = SCREEN_HEIGHT / 2;

  float cosX = cos(angleX), sinX = sin(angleX);
  float cosY = cos(angleY), sinY = sin(angleY);
  float cosZ = cos(angleZ), sinZ = sin(angleZ);

  int projectedVertices[8][2];
  for (int i = 0; i < 8; i++) {
    float x = cubeVertices[i][0];
    float y = cubeVertices[i][1];
    float z = cubeVertices[i][2];

    // Rotacao em X
    float yRotX = y * cosX - z * sinX;
    float zRotX = y * sinX + z * cosX;
    y = yRotX; z = zRotX;

    // Rotacao em Y
    float xRotY = x * cosY + z * sinY;
    float zRotY = -x * sinY + z * cosY;
    x = xRotY; z = zRotY;

    // Rotacao em Z
    float xRotZ = x * cosZ - y * sinZ;
    float yRotZ = x * sinZ + y * cosZ;
    x = xRotZ; y = yRotZ;

    projectedVertices[i][0] = int(scale * (x / (z + 4)) + xCenter);
    projectedVertices[i][1] = int(scale * (y / (z + 4)) + yCenter);
  }

  for (int i = 0; i < 4; i++) {
    display.drawLine(projectedVertices[i][0], projectedVertices[i][1], projectedVertices[(i+1) % 4][0], projectedVertices[(i+1) % 4][1], SSD1306_WHITE);
    display.drawLine(projectedVertices[i+4][0], projectedVertices[i+4][1], projectedVertices[(i+1) % 4 + 4][0], projectedVertices[(i+1) % 4 + 4][1], SSD1306_WHITE);
    display.drawLine(projectedVertices[i][0], projectedVertices[i][1], projectedVertices[i+4][0], projectedVertices[i+4][1], SSD1306_WHITE);
  }
}

void runCube() {
  if (!displayOK || !accelOK) {
    DBG_PRINTLN(F("Modo cubo requer display E acelerometro funcionando."));
    return;
  }
  accel.read();
  float angleX = map(accel.x, -2048, 2048, -180, 180) * (PI / 180.0);
  float angleY = map(accel.y, -2048, 2048, -180, 180) * (PI / 180.0);
  float angleZ = map(accel.z, -2048, 2048, -180, 180) * (PI / 180.0);

  display.clearDisplay();
  drawCube(angleX, angleY, angleZ);
  display.display();
}

// ===========================================================================
//  Setup / Loop
// ===========================================================================
enum Mode { MODE_MENU, MODE_CUBE };
Mode currentMode = MODE_CUBE;

void setup() {
  DBG_BEGIN();
  delay(200); // tempo para a serial estabilizar apos reset/upload
  DBG_PRINTLN(F("\n\n=== oled-acelerometro: inicializando ==="));

  Wire.begin();

  scanI2C();

  displayOK = initDisplay();
  accelOK   = initAccel();

  DBG_PRINTLN(F("--- Resumo ---"));
  DBG_KV("  Display ", displayOK ? F("OK") : F("FALHOU"));
  DBG_KV("  Acelerometro ", accelOK ? F("OK") : F("FALHOU"));

  // Mostra status tambem na tela, se ela funcionar
  if (displayOK) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Status:"));
    display.print(F("Display: OK"));
    display.setCursor(0, 16);
    display.print(F("Accel: "));
    display.println(accelOK ? F("OK") : F("FALHOU"));
    display.display();
    delay(1500);
  }

  printMenu();

  // Se algo falhou, comeca no menu para o usuario diagnosticar.
  currentMode = (displayOK && accelOK) ? MODE_CUBE : MODE_MENU;
}

void handleSerialCommand(char c) {
  switch (c) {
    case 's': scanI2C(); break;
    case 'd': testDisplay(); currentMode = MODE_MENU; break;
    case 'a': testAccel(); currentMode = MODE_MENU; break;
    case 'r':
      DBG_PRINTLN(F("Reinicializando..."));
      displayOK = initDisplay();
      accelOK   = initAccel();
      break;
    case 'c':
      DBG_PRINTLN(F("Modo cubo."));
      currentMode = MODE_CUBE;
      break;
    case '?':
    case '\n':
    case '\r':
      if (c == '?') printMenu();
      break;
    default:
      DBG_PRINT(F("Comando desconhecido: "));
      DBG_PRINTLN(c);
      printMenu();
      break;
  }
}

void loop() {
  // Processa comandos do menu serial a qualquer momento
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c != '\n' && c != '\r') {
      handleSerialCommand(c);
    }
  }

  if (currentMode == MODE_CUBE && displayOK && accelOK) {
    runCube();
    delay(50);
  } else {
    delay(20); // no menu, so aguarda comandos
  }
}
