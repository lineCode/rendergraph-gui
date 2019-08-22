#include "img/imgclear.h"

namespace img {

void ImgClear::execute(gfx::GraphicsBackend &gfx,
                       const ScreenSpaceContext &ctx) {
  gfx::RenderTargetView rtv{getRenderTargetImage(target_)};
  gfx.clearRenderTarget(rtv, gfx::ColorF{0.1, 0.0, 0.4});
  
}

} // namespace img