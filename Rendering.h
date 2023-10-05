#pragma once

#include <vector>

#define MATH_PI (3.14159265358979323846)
#define MATH_PIf (3.1415926f)

struct Vertex {
  float x, y;
  float r, g, b;

  constexpr bool operator==(Vertex const& other) {
    return
      x == other.x &&
      y == other.y &&
      r == other.r &&
      g == other.g &&
      b == other.b;
  }
};

class Vertices {
public:
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

public:
  Vertices();

public:
  uint16_t addVertex(Vertex vertex);
  void addIndex(uint16_t index);
  void clear();
};
