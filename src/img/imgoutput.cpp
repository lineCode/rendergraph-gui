#include "img/imgoutput.h"
#include "img/imgevaluator.h"
#include "node/description.h"

using node::Network;
using node::Node;
using node::NodeDescription;

namespace img {

ImgOutput::ImgOutput(node::Network &parent, util::StringRef name)
    : ImgNode{parent, name} {
  result_ = createInput("Result");
}

void ImgOutput::execute(ImgContext &ctx) {
  // resolve input image
  // auto&& gfx = ctx.gfx();
  // auto img = ctx.getOutputImage();
}

void ImgOutput::prepare(ImgContext &ctx) {
  // no render targets to update
}

static Node *constructor(Network &parent, util::StringRef name) {
  return new ImgOutput(parent, name);
}

void ImgOutput::registerNode() {
  ImgNetwork::registerChild("ImgOutput", "Output",
                            "Displays the input image in the viewport.",
                            constructor);
}

} // namespace img