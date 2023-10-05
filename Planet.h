#pragma once

#include <stdint.h>

#include "Vector.h"

class Planet {
public:
  Vector2   position; // In abstract units
  double    mass;     // In abstract units
  double    radius;   // In abstract units
  uint32_t  color;    // In ARGB
  Vector2   velocity; // In abstract movement units / abstract mass units

public:
  Planet(Vector2 position, double mass, double radius, uint32_t color);

public:
  // Add value to velocity
  void punch(Vector2 value);

  // Add value to position
  void move(Vector2 value);

  void tick();
};
