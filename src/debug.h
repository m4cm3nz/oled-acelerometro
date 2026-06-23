#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

// ===========================================================================
//  Suporte a debug
// ---------------------------------------------------------------------------
//  Defina DEBUG_ENABLED como 1 para habilitar as mensagens pela porta serial,
//  ou 0 para desabilitar (os macros viram "nada" e nao ocupam memoria/flash).
//
//  Tambem pode ser controlado pelo platformio.ini com:  build_flags = -D DEBUG
// ===========================================================================
#ifndef DEBUG_ENABLED
  #ifdef DEBUG
    #define DEBUG_ENABLED 1
  #else
    #define DEBUG_ENABLED 0   // padrao: desligado; ligue via build_flags no dev
  #endif
#endif

// 9600 e o valor mais confiavel no Uno (16 MHz tem ~3,5% de erro em 115200).
#define DEBUG_BAUD 9600

#if DEBUG_ENABLED
  #define DBG_BEGIN()      do { Serial.begin(DEBUG_BAUD); } while (0)
  #define DBG_PRINT(...)   Serial.print(__VA_ARGS__)
  #define DBG_PRINTLN(...) Serial.println(__VA_ARGS__)
  // Imprime "rotulo: valor" em uma linha
  #define DBG_KV(label, value) do { Serial.print(F(label)); Serial.print(F(": ")); Serial.println(value); } while (0)
#else
  #define DBG_BEGIN()      do {} while (0)
  #define DBG_PRINT(...)   do {} while (0)
  #define DBG_PRINTLN(...) do {} while (0)
  #define DBG_KV(label, value) do {} while (0)
#endif

#endif // DEBUG_H
