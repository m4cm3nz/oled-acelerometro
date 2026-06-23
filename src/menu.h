#ifndef MENU_H
#define MENU_H

#include "shapes.h"

// ===========================================================================
//  Menu de diagnostico pela porta serial e modos de execucao.
// ===========================================================================

enum Mode { MODE_MENU, MODE_SHAPE };

extern Mode currentMode;
extern const Shape *currentShape;  // forma desenhada no MODE_SHAPE

void printMenu();
void handleSerialCommand(char c);

// Rotinas de teste individuais (acionadas pelo menu serial).
void testDisplay();
void testAccel();

#endif // MENU_H
