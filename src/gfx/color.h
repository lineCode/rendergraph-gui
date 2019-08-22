#pragma once
#include <cstdint>

namespace gfx {
struct ColorF {
  double r = 0.0;
  double g = 0.0;
  double b = 0.0;
  double a = 0.0;

  ColorF() = default;

  ColorF(double r, double g, double b, double a = 1.0) : r{ r }, g{ g }, b{ b }, a{ a } 
  {}

  static ColorF fromU8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
	  return ColorF{
		  (double)r / 255.0,
		  (double)g / 255.0,
		  (double)b / 255.0,
		  (double)a / 255.0,
	  };
  }

  friend constexpr bool operator==(const ColorF& lhs, const ColorF &rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
  }
};

} // namespace gfx