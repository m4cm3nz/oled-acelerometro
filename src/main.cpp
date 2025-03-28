#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_MMA8452Q.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

MMA8452Q accel;

// Coordenadas 3D do cubo
float cubeVertices[8][3] = {
  {1, 1, 1}, {-1, 1, 1}, {-1, -1, 1}, {1, -1, 1},  // Frente
  {1, 1, -1}, {-1, 1, -1}, {-1, -1, -1}, {1, -1, -1}  // Trás
};

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Falha na inicialização do display SSD1306"));
    for (;;);
  }

  if (!accel.begin()) {
    Serial.println("Falha na inicialização do MMA8452Q");
    while (1);
  }


  display.clearDisplay();
  display.display();
}


void drawCube(float angleX, float angleY, float angleZ) {
  int scale = 60;
  int xCenter = SCREEN_WIDTH / 2;
  int yCenter = SCREEN_HEIGHT / 2;

  // Matrizes de rotação nos eixos X, Y e Z
  float cosX = cos(angleX);
  float sinX = sin(angleX);
  float cosY = cos(angleY);
  float sinY = sin(angleY);
  float cosZ = cos(angleZ);
  float sinZ = sin(angleZ);

  // Projeção 2D das coordenadas 3D com rotação
  int projectedVertices[8][2];
  for (int i = 0; i < 8; i++) {
    // Coordenadas originais do cubo
    float x = cubeVertices[i][0];
    float y = cubeVertices[i][1];
    float z = cubeVertices[i][2];

    // Aplicar rotação no eixo X
    float yRotX = y * cosX - z * sinX;
    float zRotX = y * sinX + z * cosX;
    y = yRotX;
    z = zRotX;

    // Aplicar rotação no eixo Y
    float xRotY = x * cosY + z * sinY;
    float zRotY = -x * sinY + z * cosY;
    x = xRotY;
    z = zRotY;

    // Aplicar rotação no eixo Z
    float xRotZ = x * cosZ - y * sinZ;
    float yRotZ = x * sinZ + y * cosZ;
    x = xRotZ;
    y = yRotZ;

    // Projeção 2D
    projectedVertices[i][0] = int(scale * (x / (z + 4)) + xCenter);
    projectedVertices[i][1] = int(scale * (y / (z + 4)) + yCenter);
  }

  // Desenhar as arestas do cubo
  for (int i = 0; i < 4; i++) {
    display.drawLine(projectedVertices[i][0], projectedVertices[i][1], projectedVertices[(i+1) % 4][0], projectedVertices[(i+1) % 4][1], SSD1306_WHITE); // Frente
    display.drawLine(projectedVertices[i+4][0], projectedVertices[i+4][1], projectedVertices[(i+1) % 4 + 4][0], projectedVertices[(i+1) % 4 + 4][1], SSD1306_WHITE); // Trás
    display.drawLine(projectedVertices[i][0], projectedVertices[i][1], projectedVertices[i+4][0], projectedVertices[i+4][1], SSD1306_WHITE); // Conexões entre frente e trás
  }
}


void loop() {
  // Ler dados do acelerômetro
  accel.read();

  // Calcular os ângulos de rotação com base nos valores do acelerômetro
  float angleX = map(accel.x, -2048, 2048, -180, 180) * (PI / 180.0);
  float angleY = map(accel.y, -2048, 2048, -180, 180) * (PI / 180.0);
  float angleZ = map(accel.z, -2048, 2048, -180, 180) * (PI / 180.0);

  // Limpar a tela e desenhar o cubo rotacionado
  display.clearDisplay();
  drawCube(angleX, angleY, angleZ);
  display.display();

  delay(50); // Pequeno atraso para observar as mudanças
}