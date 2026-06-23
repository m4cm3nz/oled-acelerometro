#ifndef SHAPES_H
#define SHAPES_H

#include <stdint.h>

// ===========================================================================
//  Formas 3D em wireframe (arames) projetadas no display.
// ---------------------------------------------------------------------------
//  Uma forma e descrita por seus vertices (pontos 3D) e suas arestas (pares
//  de indices de vertices que devem ser ligados por uma linha). Isso permite
//  adicionar novas formas apenas com dados, sem duplicar o codigo de
//  rotacao/projecao.
//
//  Limite: ate 8 vertices por forma (suficiente para cubo e triangulo).
// ===========================================================================
#define SHAPE_MAX_VERTICES 8

struct Shape {
  const float (*vertices)[3];  // vertexCount x 3 (x, y, z)
  uint8_t vertexCount;
  const uint8_t (*edges)[2];   // edgeCount x 2 (indices de vertices)
  uint8_t edgeCount;
};

extern const Shape SHAPE_CUBE;
extern const Shape SHAPE_TRIANGLE;  // tetraedro (piramide triangular)

// Rotaciona a forma pelos angulos dados (radianos), projeta em perspectiva e
// desenha as arestas no display. NAO chama clearDisplay()/display().
void drawShape(const Shape &shape, float angleX, float angleY, float angleZ);

#endif // SHAPES_H
