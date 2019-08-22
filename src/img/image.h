#pragma once
#include "gfx/gfx.h"
#include "gfx/image.h"
#include "gfx/sampler.h"
#include "util/filesystem.h"

namespace img {

class Image {
public:
  /// Creates a default (empty) image. Does not allocate any storage (CPU or
  /// GPU).
  Image();
  /// Creates an image with the specified format and dimension.
  Image(gfx::Format format, int w, int h);
  Image(const gfx::ImageDesc &desc);

  /// Returns the GPU image associated with this image. If no GPU resource has
  /// been created for this image, then it will create one on the fly.
  gfx::Image &getGPUImage(gfx::GraphicsBackend &gfx);

private:
  gfx::ImageDesc desc_;
  gfx::Image gpu_;
  // util::path path_;
};

} // namespace img