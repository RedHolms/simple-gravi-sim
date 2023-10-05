#include "Rendering.h"

Vertices::Vertices() {}

uint16_t Vertices::addVertex(Vertex vertex) {
  for (uint16_t i = 0; i < vertices.size(); ++i) {
    if (vertices[i] == vertex) {
      indices.push_back(i);
      return i;
    }
  }

  uint16_t i = vertices.size();

  indices.push_back(i);
  vertices.push_back(vertex);

  return i;
}

void Vertices::addIndex(uint16_t index) {
  indices.push_back(index);
}

void Vertices::clear() {
  vertices.clear();
  indices.clear();
}
