#include "img/imgclear.h"
#include "node/blueprint.h"
#include "node/param.h"

using node::Blueprint;
using node::Network;
using node::Node;
using node::Param;
using node::ParamDesc;
using node::ParamName;

namespace img {

static ParamDesc P_CLEAR_COLOR = node::paramColorRGBA(ParamName("color"), "The color to fill the image with.");
static ParamDesc PARAMS[] = { P_CLEAR_COLOR };

void ImgClear::execute(gfx::GraphicsBackend &gfx,
                       const ScreenSpaceContext &ctx) {
  gfx::RenderTargetView rtv{getRenderTargetImage(target_)};
  gfx.clearRenderTarget(rtv, gfx::ColorF{0.1, 0.0, 0.4});
}

void ImgClear::registerBlueprint() {
  ImgNetwork::registerChild(new Blueprint(
      "ImgClear", "Clear", "Creates an image cleared to the given color.", "",
	  PARAMS,
      [](Network &parent, std::string name, Blueprint &blueprint) -> Node * {
        return new ImgClear(parent, name, blueprint);
      }));
}

} // namespace img