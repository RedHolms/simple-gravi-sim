#include "Ticker.h"

#include <GLFW/glfw3.h>

Ticker::Ticker(uint32_t tickRate) {
  previous_update = glfwGetTime();
  tick_period = 1.0 / tickRate;
  ticks = 0.0;
}

uint32_t Ticker::tick() noexcept {
  double delta = glfwGetTime() - previous_update;
  previous_update = glfwGetTime();

  ticks += delta / tick_period;

  uint32_t actual_ticks = static_cast<uint32_t>(ticks);
  ticks -= actual_ticks;

  return actual_ticks;
}
