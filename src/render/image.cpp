#include "render/image.h"

namespace render {

/// Creates a default (empty) image. Does not allocate any storage (CPU or GPU).
Image::Image() {}

/// Creates an image with the specified format and dimension.
Image::Image(gfx::Format format, int w, int h) {
  desc_.format = format;
  desc_.width = w;
  desc_.height = h;
  desc_.dimensions = gfx::ImageDimensions::Image2D;
}

Image::Image(const gfx::ImageDesc &desc) {}

gfx::Image &Image::getGPUImage(gfx::GraphicsBackend *gfx) {
  if (!gpu_) {
    gpu_ = gfx::Image{gfx, desc_};
    // TODO: handle possible errors (out of memory conditions, etc.)
  }
  return gpu_;
}

} // namespace render