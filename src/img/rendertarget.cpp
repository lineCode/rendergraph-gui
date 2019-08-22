#include "img/rendertarget.h"
#include "img/imgnode.h"

namespace img {

class BackingImage {
  friend class RenderTargetCache;

public:
  BackingImage(const gfx::ImageDesc &desc, gfx::ImageHandle handle)
      : desc_{desc}, handle_{handle}, useCount_{0} {}

private:
  gfx::ImageDesc desc_;
  gfx::ImageHandle handle_;
  int useCount_ = 0;
};

class RenderTarget {
public:
  // description of the image
  gfx::ImageDesc desc;
  BackingImage *image;
};

RenderTargetCache::RenderTargetCache(gfx::GraphicsBackend &backend)
    : backend_{backend} {}

RenderTargetCache::~RenderTargetCache() {}

RenderTarget *
RenderTargetCache::createRenderTarget(const gfx::ImageDesc &desc) {
  return nullptr;
}

void RenderTargetCache::setRenderTargetDesc(RenderTarget *renderTarget,
                                            const gfx::ImageDesc &desc) {
  if (renderTarget->desc == desc) {
    return;
  }
  if (renderTarget->image) {
    renderTarget->image->useCount_--;
  }
  renderTarget->desc = desc;
  renderTarget->image = nullptr;
}

const gfx::ImageDesc &
RenderTargetCache::getRenderTargetDesc(RenderTarget *renderTarget) {
  return renderTarget->desc;
}

gfx::ImageHandle RenderTargetCache::getImage(RenderTarget *renderTarget) {
  if (!renderTarget->image) {
    assignImage(renderTarget);
  }

  return renderTarget->image->handle_;
}

void RenderTargetCache::assignImage(RenderTarget *rt) {
  // For now, we simply look for an unused image in the cache and use it.
  // If there are no unused images in the cache, we create a new one.
  // In the future, we might want to share images between render target if
  // we can prove that the render targets are not used at the same time in the
  // graph (see "Frame graphs"). In theory, we can also reorder the passes
  // within the frame to maximize aliasing (and minimize the amount of memory
  // required). However, it's a complicated problem.
  for (int i = 0; i < images_.size(); ++i) {
    if (images_[i]->useCount_ == 0 && images_[i]->desc_ == rt->desc) {
      images_[i]->useCount_++;
      rt->image = images_[i].get();
      return;
    }
  }
  // allocate a new image
  auto handle = backend_.createImage(rt->desc);
  auto img = std::make_unique<BackingImage>(rt->desc, handle);
  rt->image = img.get();
  images_.push_back(std::move(img));
}

void RenderTargetCache::deleteRenderTarget(RenderTarget* renderTarget) {
	// TODO
}

RenderTargetCache::Ptr RenderTargetCache::make(gfx::GraphicsBackend &backend) {
  return std::make_unique<RenderTargetCache>(backend);
}

} // namespace img