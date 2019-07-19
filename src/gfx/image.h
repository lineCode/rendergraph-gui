#pragma once
#include <cstdint>
#include "util/bitflags.h"
#include "gfx/imageformat.h"

namespace gfx {
enum class ImageDimensions { Image1D, Image2D, Image2DArray, Image3D };

enum class ImageUsageFlags : uint32_t {
  ColorAttachment = 0b00000001,
  DepthAttachment = 0b00000010,
  InputAttachment = 0b00000100,
  Storage = 0b00001000,
  Sampled = 0b00010000,
};

ENUM_BIT_FLAGS_OPERATORS(ImageUsageFlags)

struct ImageDesc {
	ImageDimensions dimensions;
	ImageFormat format;
	int width;
	int height;
	int depth;
	int arrayLayerCount;
	int mipMapCount;
	int sampleCount;
	ImageUsageFlags usage;
};

class Image {
public:
  virtual int width() = 0;
  virtual int height() = 0;
  virtual int depth() = 0;
  virtual int format() = 0;
};
} // namespace gfx