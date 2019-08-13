#pragma once
#include "gfx/gfx.h"
#include "gfx/image.h"
#include "node.h"

namespace render {

class ScreenSpaceNode;
struct ScreenSpaceContext;

struct RenderTargetStorage {
  gfx::ImageDesc desc;
  gfx::Image image;
};

class RenderTarget {
public:
  using Ptr = std::unique_ptr<RenderTarget>;

  RenderTarget(const gfx::ImageDesc &desc)
      : desc_{desc} 
  {
  }

  const gfx::ImageDesc &desc() const { return desc_; }

  gfx::ImageHandle getImage(const ScreenSpaceContext &ctx); // TODO

private:
  gfx::ImageDesc desc_;
  std::shared_ptr<RenderTargetStorage> storage_;
};

} // namespace render