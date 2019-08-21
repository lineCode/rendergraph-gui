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

  RenderTargetCache(gfx::GraphicsBackend *backend);
  ~RenderTargetCache();

  RenderTarget *createRenderTarget(const gfx::ImageDesc &desc);

  void setRenderTargetDesc(RenderTarget *renderTarget,
                           const gfx::ImageDesc &desc);
  gfx::ImageHandle getImage(RenderTarget *renderTarget);

  RenderTargetCache::Ptr RenderTargetCache::make(gfx::GraphicsBackend *backend);

private:
  void assignImage(RenderTarget *rt);

  gfx::GraphicsBackend *backend_;
  std::vector<std::unique_ptr<RenderTarget>> renderTargets_;
  std::vector<std::unique_ptr<BackingImage>> images_;
};

} // namespace img