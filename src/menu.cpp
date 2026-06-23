#include <Arduino.h>
#include "menu.h"
#include "devices.h"
#include "config.h"
#include "debug.h"

Mode currentMode = MODE_SHAPE;
const Shape *currentShape = &SHAPE_CUBE;

// ===========================================================================
//  Testes individuais
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

// ===========================================================================
//  Menu
// ===========================================================================
void printMenu() {
  DBG_PRINTLN(F(""));
  DBG_PRINTLN(F("===== MENU DE DIAGNOSTICO ====="));
  DBG_PRINTLN(F("  s = scan do barramento I2C"));
  DBG_PRINTLN(F("  d = testar display"));
  DBG_PRINTLN(F("  a = testar acelerometro"));
  DBG_PRINTLN(F("  r = reinicializar dispositivos"));
  DBG_PRINTLN(F("  c = animar o cubo (modo normal)"));
  DBG_PRINTLN(F("  t = animar o triangulo (tetraedro)"));
  DBG_PRINTLN(F("  (chacoalhe a placa para alternar a forma)"));
  DBG_PRINTLN(F("  ? = mostrar este menu"));
  DBG_PRINTLN(F("==============================="));
}

static void selectShape(const Shape *shape, const __FlashStringHelper *name) {
  currentShape = shape;
  currentMode = MODE_SHAPE;
  accelResetFilter();  // recomeca o filtro ao trocar de forma
  DBG_PRINT(F("Modo forma: "));
  DBG_PRINTLN(name);
}

void toggleShape() {
  if (currentShape == &SHAPE_CUBE) {
    selectShape(&SHAPE_TRIANGLE, F("triangulo"));
  } else {
    selectShape(&SHAPE_CUBE, F("cubo"));
  }
}

void handleSerialCommand(char c) {
  switch (c) {
    case 's': scanI2C(); break;
    case 'd': testDisplay(); currentMode = MODE_MENU; break;
    case 'a': testAccel();   currentMode = MODE_MENU; break;
    case 'r':
      DBG_PRINTLN(F("Reinicializando..."));
      displayOK = initDisplay();
      accelOK   = initAccel();
      break;
    case 'c': selectShape(&SHAPE_CUBE,     F("cubo"));      break;
    case 't': selectShape(&SHAPE_TRIANGLE, F("triangulo")); break;
    case '?':
      printMenu();
      break;
    default:
      DBG_PRINT(F("Comando desconhecido: "));
      DBG_PRINTLN(c);
      printMenu();
      break;
  }
}
