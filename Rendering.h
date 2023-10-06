#pragma once

#include <array>
#include <vector>
#include <string>

#include <glad/gl.h>

#include "Vector.h"

#define PI 3.141592653589793f
#define DOUBLE_PI (PI * 2)

struct ColorRGB {
  float r, g, b;

  constexpr ColorRGB(float r, float g, float b)
    : r(r), g(g), b(b) {}

  constexpr ColorRGB(uint32_t color)
    : r( ((color >> 16) & 0xFF) / 255.0 ),
      g( ((color >>  8) & 0xFF) / 255.0 ),
      b( ( color        & 0xFF) / 255.0 ) {}

  constexpr operator uint32_t() const noexcept {
    return toRGB();
  }

  constexpr uint32_t toRGB() const noexcept {
    return
      (int)(r * 255) << 16 |
      (int)(g * 255) << 8  |
      (int)(b * 255);
  }

  constexpr bool operator==(ColorRGB const& other) {
    return
      r == other.r &&
      g == other.g &&
      b == other.b;
  }
};

struct Vertex {
  Vector2 pos;
  ColorRGB col;

  constexpr Vertex(Vector2 pos, ColorRGB col)
    : pos(pos), col(col) {}

  constexpr bool operator==(Vertex const& other) {
    return
      pos == other.pos &&
      col == other.col;
  }
};

using Index = uint32_t;

class Shaders {
public:
  std::array<GLuint, 2> shaders;
  GLuint program;

  std::string lastError;

public:
  Shaders();
  ~Shaders();

public:
  bool compile(GLenum shaderType, const char* code);
  bool link();

  void select();
};

class Renderer {
public:
  std::vector<Vertex> vertices;
  std::vector<Index> indices;

  Vector2 viewport;

  Shaders shaders;

  GLuint vertexArray;

  GLuint verticesBuffer;
  GLuint indicesBuffer;

public:
  Renderer();

public:
  void init();

  void beginFrame();
  void render();

  void clearBuffers();

  void setViewport(Vector2 viewport);

  void drawCircle(Vector2 center, float radius, uint32_t colorRGB, uint32_t segments_number = 40);
  void drawPoligon(Vector2 p1, Vector2 p2, Vector2 p3, uint32_t colorRGB);

  Index addNewVertex(Vertex vertex);
  void addIndex(Index index);

  // Note: very not efficient
  Index addVertex(Vertex vertex);

private:
  void initShaders();
  void initBuffers();
};
