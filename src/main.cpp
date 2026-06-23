#include <Arduino.h>
#include <math.h>
#include "config.h"
#include "debug.h"
#include "devices.h"
#include "shapes.h"
#include "menu.h"

// ===========================================================================
//  oled-acelerometro
//  Forma 3D (cubo ou triangulo) girando num OLED SSD1306 conforme a
//  inclinacao de um acelerometro MMA8452Q, com menu de diagnostico serial.
//
//  A logica esta dividida em modulos:
//    config.h   -> constantes de hardware
//    devices.*  -> display, acelerometro, scan I2C e leitura suavizada
//    shapes.*   -> formas 3D (vertices/arestas) e renderizacao
//    menu.*     -> menu serial, testes e modo atual
// ===========================================================================

// Desenha a forma atual usando a leitura suavizada do acelerometro.
static void runShape() {
  float ax, ay, az;
  accelReadSmoothed(ax, ay, az);

  // Mapeia a faixa bruta (-2048..2048) para -PI..PI radianos.
  float angleX = (ax / 2048.0f) * PI;
  float angleY = (ay / 2048.0f) * PI;
  float angleZ = (az / 2048.0f) * PI;

  display.clearDisplay();
  drawShape(*currentShape, angleX, angleY, angleZ);
  display.display();
}

void setup() {
  DBG_BEGIN();
  delay(200); // tempo para a serial estabilizar apos reset/upload
  DBG_PRINTLN(F("\n\n=== oled-acelerometro: inicializando ==="));

  busBegin();
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
  currentMode = (displayOK && accelOK) ? MODE_SHAPE : MODE_MENU;
}

void loop() {
  // Processa comandos do menu serial a qualquer momento
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c != '\n' && c != '\r') {
      handleSerialCommand(c);
    }
  }

  if (currentMode == MODE_SHAPE && displayOK && accelOK) {
    runShape();
    delay(50);
  } else {
    delay(20); // no menu, so aguarda comandos
  }
}
