#include "render/rendertarget.h"
#include "render/screenspacenode.h"

namespace render {

	/*
RenderTarget *RenderTarget::make(ScreenSpaceNode *parent, std::string name,
                                 const gfx::ImageDesc &desc) {
  return static_cast<RenderTarget *>(
	  parent->addChild(std::make_unique<RenderTarget>(desc, std::move(name))));
}*/

gfx::ImageHandle RenderTargetOutput::getImage(const ScreenSpaceContext &ctx) {
  return 0;
}

} // namespace render