#include "img/imgclear.h"
#include "img/imgevaluator.h"
#include "node/description.h"
#include "node/param.h"

using node::Network;
using node::Node;
using node::NodeDescription;
using node::Param;
using node::ParamDesc;

namespace img {

static Node *constructor(Network &parent, util::StringRef name) {
  return new ImgClear(parent, name);
}

static const char *OUTPUT_NAME = "output";

void ImgClear::registerNode() {
  ImgNetwork::registerChild("ImgClear", "Clear",
                            "Creates an image cleared to the given color.",
                            constructor);
}

ImgClear::ImgClear(node::Network &parent, util::StringRef name)
    : ImgNode{parent, name} {
  gfx::ImageDesc desc;
  createOutput(OUTPUT_NAME);

  auto colorParam = node::paramColorRGBA("color", "Clear Color", "Clear Color");
  createParameter(colorParam);
}

void ImgClear::prepare(ImgContext &ctx) {
  // update the description of the render target
  int w, h;
  ctx.defaultImageSize(w, h);
  gfx::ImageDesc targetDesc;
  targetDesc.width = w;
  targetDesc.height = h;
  ctx.setRenderTargetDesc(OUTPUT_NAME, targetDesc);
}

void ImgClear::execute(ImgContext &ctx) {
  // evaluate parameters
  auto &&color = evalParam("color").asRealArray();

  // get the concrete image associated to the render target
  auto targetView = ctx.getRenderTargetView(OUTPUT_NAME);

  auto& gfx = ctx.gfx();
  gfx.clearRenderTarget(targetView,
                        gfx::ColorF{color[0], color[1], color[2], color[3]});
}

} // namespace img