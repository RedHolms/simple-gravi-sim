#pragma once

#include <stdint.h>

#include "Vector.h"

class Planet {
public:
  Vector2   position;
  double    mass;     // KG
  double    radius;   // pixels
  uint32_t  color;    // RGB
  Vector2   velocity; // pixels / KG * second

public:
  Planet(Vector2 position, double mass, double radius, uint32_t color);

public:
  void punch(Vector2 value);
  void move(Vector2 value);

  void tick();
};
