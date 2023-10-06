#include "Planet.h"

Planet::Planet(Vector2 position, double mass, double radius, uint32_t color) {
  this->position = position;
  this->mass = mass;
  this->radius = radius;
  this->color = color;
  this->velocity = Vector2();
}

void Planet::tick() {
  move(velocity / mass);
}

void Planet::punch(Vector2 value) {
  velocity += value;
}

void Planet::move(Vector2 value) {
  position += value;
}
