#include "img/rendertarget.h"
#include "img/screenspacenode.h"

namespace img {

/*
RenderTarget *RenderTarget::make(ScreenSpaceNode *parent, std::string name,
                         const gfx::ImageDesc &desc) {
return static_cast<RenderTarget *>(
  parent->addChild(std::make_unique<RenderTarget>(desc, std::move(name))));
}*/

gfx::ImageHandle RenderTarget::getImage(const ScreenSpaceContext &ctx) {
  return 0;
}

} // namespace img