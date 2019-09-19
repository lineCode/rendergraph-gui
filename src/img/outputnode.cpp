#include "img/outputnode.h"
#include "node/template.h"

using node::Network;
using node::Node;
using node::NodeTemplate;

namespace img {

ImgOutput::ImgOutput(node::Network &parent, util::StringRef name,
                     node::NodeTemplate &tpl)
    : ImgNode{parent, name, tpl } {
  result_ = createInput("Result");
}

void ImgOutput::execute(gfx::GraphicsBackend &    gfx,
                        const ScreenSpaceContext &ctx) {
  // resolve image
  // gfx->presentToScreen()
}

static Node *createImgOutput(Network &parent, util::StringRef name,
                             NodeTemplate &blueprint) {
  return new ImgOutput(parent, name, blueprint);
}

void ImgOutput::registerTemplate() {
  ImgNetwork::registerTemplate("ImgOutput", "Output",
                               "Displays the input image in the viewport.", "",
                               nullptr, nullptr,nullptr, createImgOutput);
}

} // namespace img