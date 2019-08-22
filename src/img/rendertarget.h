#pragma once
#include "gfx/gfx.h"
#include "gfx/image.h"
#include "node/node.h"
#include <memory>

namespace img {

class RenderTarget;
class BackingImage;

class RenderTargetCache {
public:
  using Ptr = std::unique_ptr<RenderTargetCache>;

  RenderTargetCache(gfx::GraphicsBackend &backend);
  ~RenderTargetCache();

  /// Creates a new render target. 
  /// The memory for the render target might not be allocated on the spot.
  RenderTarget *createRenderTarget(const gfx::ImageDesc &desc);

  void deleteRenderTarget(RenderTarget* renderTarget);

  /// Sets the description of the render target. This invalidates image handles returned by `getImage()` for the specified render target.
  void setRenderTargetDesc(RenderTarget *renderTarget,
                           const gfx::ImageDesc &desc);

  /// Returns the description of the render target.
  const gfx::ImageDesc& getRenderTargetDesc(RenderTarget *renderTarget);
  
  /// Returns the GPU image handle of the render target. 
  /// If the memory for the image has not been allocated yet, this function allocates it.
  gfx::ImageHandle getImage(RenderTarget *renderTarget);

  /// Constructor.
  RenderTargetCache::Ptr RenderTargetCache::make(gfx::GraphicsBackend &backend);

private:
  void assignImage(RenderTarget *rt);

  gfx::GraphicsBackend &backend_;
  std::vector<std::unique_ptr<RenderTarget>> renderTargets_;
  std::vector<std::unique_ptr<BackingImage>> images_;
};

} // namespace img