#include "img/imgclear.h"
#include "node/param.h"
#include "node/template.h"

using node::Network;
using node::Node;
using node::NodeTemplate;
using node::Param;
using node::ParamDesc;

namespace img {

static Node *createImgClear(Network &parent, util::StringRef name,
                            NodeTemplate &blueprint) {
  return new ImgClear(parent, name, blueprint);
}

void ImgClear::registerTemplate() {
  ImgNetwork::registerTemplate("ImgClear", "Clear",
                               "Creates an image cleared to the given color.",
                               "", nullptr, nullptr, nullptr, createImgClear);
}

ImgClear::ImgClear(node::Network &parent, util::StringRef name, node::NodeTemplate &tpl)
	: ImgNode{ parent, name, tpl }
{
	createOutput("image");
	auto colorParam = node::paramColorRGBA("color", "Clear Color", "Clear Color");
	createParameter(colorParam);
}

void ImgClear::execute(gfx::GraphicsBackend &    gfx,
                       const ScreenSpaceContext &ctx)
{
	evalParam("color");

  gfx::RenderTargetView rtv{getRenderTargetImage(target_)};
  gfx.clearRenderTarget(rtv, gfx::ColorF{0.1, 0.0, 0.4});
}

} // namespace img