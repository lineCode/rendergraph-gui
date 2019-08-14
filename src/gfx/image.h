#pragma once
#include "gfx/format.h"
#include "util/bitflags.h"
#include <cstdint>

namespace gfx {
enum class ImageDimensions { Image1D, Image2D, Image2DArray, Image3D };

enum class ImageUsageFlags : uint32_t {
  ColorAttachment = 0b00000001,
  DepthAttachment = 0b00000010,
  InputAttachment = 0b00000100,
  Storage = 0b00001000,
  Sampled = 0b00010000,
  All = 0b00011111,
};

ENUM_BIT_FLAGS_OPERATORS(ImageUsageFlags)

struct ImageDesc {
  ImageDimensions dimensions = ImageDimensions::Image2D;
  Format format = gfx::Format::R8G8B8A8_SRGB;
  int width = 1;
  int height = 1;
  int depth = 1;
  int arrayLayerCount = 1;
  int mipMapCount = 1;
  int sampleCount = 1;
  ImageUsageFlags usage = ImageUsageFlags::All;

  /// Comparison operator
  constexpr bool operator==(const ImageDesc &rhs) const {
    return dimensions == rhs.dimensions && format == rhs.format &&
           width == rhs.width && height == rhs.height && depth == rhs.depth &&
           arrayLayerCount == rhs.arrayLayerCount &&
           mipMapCount == rhs.mipMapCount && sampleCount == rhs.sampleCount &&
           usage == rhs.usage;
  }

  constexpr bool operator!=(const ImageDesc &rhs) const {
	  return !(*this == rhs);
  }
};

} // namespace gfx