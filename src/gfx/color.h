#pragma once

namespace gfx {
struct ColorF {
  float r = 0.0;
  float g = 0.0;
  float b = 0.0;
  float a = 0.0;

  friend constexpr bool operator==(const ColorF& lhs, const ColorF &rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
  }
};

} // namespace gfx