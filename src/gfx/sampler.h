#pragma once
#include "gfx/color.h"
#include "util/bitflags.h"

namespace gfx {


struct SamplerDesc {
  enum class AddressMode { Repeat, Clamp, Border, Mirror };

  enum class Filter {
    Nearest,
    Linear,
  };

  enum class MipMapMode {
	  None,
    Nearest,
    Linear,
  };

  /// U-coordinate repeat mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
  AddressMode addrU = AddressMode::Repeat;
  /// V-coordinate repeat mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
  AddressMode addrV = AddressMode::Repeat;
  /// W-coordinate repeat mode (GL_REPEAT, GL_CLAMP_TO_EDGE, etc.)
  AddressMode addrW = AddressMode::Repeat;
  /// Minification filtering mode (see OpenGL docs for allowed options)
  Filter minFilter = Filter::Nearest;
  /// Magnification filtering mode (see OpenGL docs for allowed options)
  Filter magFilter = Filter::Nearest;
  MipMapMode mipMapMode = MipMapMode::Nearest;
  /// Border color for use with the GL_CLAMP_TO_BORDER repeat mode
  ColorF borderColor = ColorF{
      0.0f, 0.0f, 0.0f,
       0.0f}; // vec4 is not constexpr, so use an array instead

  /// Comparison operator
  constexpr bool operator==(const SamplerDesc &rhs) const {
    return addrU == rhs.addrU && addrV == rhs.addrV && addrW == rhs.addrW &&
           minFilter == rhs.minFilter && magFilter == rhs.magFilter &&
           mipMapMode == rhs.mipMapMode && borderColor == rhs.borderColor;
  }
};

} // namespace gfx
