#include <Arduino.h>
#include <math.h>
#include "shapes.h"
#include "devices.h"
#include "config.h"

// ===========================================================================
//  Cubo: 8 vertices, 12 arestas
// ===========================================================================
static const float cubeVertices[8][3] = {
  { 1,  1,  1}, {-1,  1,  1}, {-1, -1,  1}, { 1, -1,  1},  // frente (z = +1)
  { 1,  1, -1}, {-1,  1, -1}, {-1, -1, -1}, { 1, -1, -1}   // tras   (z = -1)
};
static const uint8_t cubeEdges[12][2] = {
  {0, 1}, {1, 2}, {2, 3}, {3, 0},  // face da frente
  {4, 5}, {5, 6}, {6, 7}, {7, 4},  // face de tras
  {0, 4}, {1, 5}, {2, 6}, {3, 7}   // ligacoes frente-tras
};

const Shape SHAPE_CUBE = { cubeVertices, 8, cubeEdges, 12 };

// ===========================================================================
//  Triangulo (tetraedro / piramide triangular): 4 vertices, 6 arestas
// ---------------------------------------------------------------------------
//  Vertices de um tetraedro regular centrado na origem.
// ===========================================================================
static const float triVertices[4][3] = {
  { 1,  1,  1},
  { 1, -1, -1},
  {-1,  1, -1},
  {-1, -1,  1}
};
static const uint8_t triEdges[6][2] = {
  {0, 1}, {0, 2}, {0, 3},  // do topo para a base
  {1, 2}, {2, 3}, {3, 1}   // base triangular
};

const Shape SHAPE_TRIANGLE = { triVertices, 4, triEdges, 6 };

// ===========================================================================
//  Renderizacao
// ---------------------------------------------------------------------------
//  scale 36 mantem a forma dentro dos 64px de altura da tela (com 60 o cubo
//  estourava as bordas superior/inferior).
// ===========================================================================
void drawShape(const Shape &shape, float angleX, float angleY, float angleZ) {
  const int scale = 36;
  const int xCenter = SCREEN_WIDTH / 2;
  const int yCenter = SCREEN_HEIGHT / 2;

  const float cosX = cos(angleX), sinX = sin(angleX);
  const float cosY = cos(angleY), sinY = sin(angleY);
  const float cosZ = cos(angleZ), sinZ = sin(angleZ);

  int projected[SHAPE_MAX_VERTICES][2];
  for (uint8_t i = 0; i < shape.vertexCount; i++) {
    float x = shape.vertices[i][0];
    float y = shape.vertices[i][1];
    float z = shape.vertices[i][2];

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

    // Projecao em perspectiva (z + 4 afasta a forma da camera)
    projected[i][0] = int(scale * (x / (z + 4)) + xCenter);
    projected[i][1] = int(scale * (y / (z + 4)) + yCenter);
  }

  for (uint8_t e = 0; e < shape.edgeCount; e++) {
    uint8_t a = shape.edges[e][0];
    uint8_t b = shape.edges[e][1];
    display.drawLine(projected[a][0], projected[a][1],
                     projected[b][0], projected[b][1], SSD1306_WHITE);
  }
}
