#pragma once

#include <stdint.h>

class Ticker {
public:
  double previous_update;
  double tick_period;
  double ticks;

public:
  Ticker(uint32_t tickRate);

public:
  uint32_t tick() noexcept;
};
