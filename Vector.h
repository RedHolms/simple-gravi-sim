#pragma once

#include <math.h>

struct Vector2 {
  double x, y;

  constexpr Vector2()
    : x(0), y(0) {}

  explicit constexpr Vector2(double v)
    : x(v), y(v) {}

  constexpr Vector2(double x, double y)
    : x(x), y(y) {}

  inline double length() const noexcept {
    return sqrt( (x * x) + (y * y) );
  }

  inline Vector2 normalize() const noexcept {
    double l = length();
    return Vector2( x / l, y / l );
  }

  constexpr Vector2& operator+=(Vector2 const& v) noexcept {
    x += v.x; y += v.y;
    return *this;
  }

  constexpr Vector2 operator+(Vector2 const& v) const noexcept {
    return Vector2( x + v.x, y + v.y );
  }

  constexpr Vector2& operator-=(Vector2 const& v) noexcept {
    x -= v.x; y -= v.y;
    return *this;
  }

  constexpr Vector2 operator-(Vector2 const& v) const noexcept {
    return Vector2( x - v.x, y - v.y );
  }

  constexpr Vector2 operator*=(double scaler) noexcept {
    x *= scaler; y *= scaler;
    return *this;
  }

  constexpr Vector2 operator*(double scaler) const noexcept {
    return Vector2( x * scaler, y * scaler );
  }

  constexpr Vector2 operator/=(double scaler) noexcept {
    x /= scaler; y /= scaler;
    return *this;
  }

  constexpr Vector2 operator/(double scaler) const noexcept {
    return Vector2( x / scaler, y / scaler );
  }

  constexpr Vector2 operator-() const noexcept {
    return Vector2( -x, -y );
  }
};
