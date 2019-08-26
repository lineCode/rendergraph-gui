#include "img/outputnode.h"
#include "node/blueprint.h"

using node::Blueprint;
using node::Network;
using node::Node;

namespace img {

void ImgOutput::execute(gfx::GraphicsBackend &gfx,
                        const ScreenSpaceContext &ctx) {
  // resolve image
  // gfx->presentToScreen()
}

void ImgOutput::registerBlueprint() {
  ImgNetwork::registerChild(new Blueprint(
      "ImgOutput", "Output", "Displays the input image in the viewport.", "",
	  nullptr,
      [](Network &parent, std::string name, Blueprint &blueprint) -> Node * {
        return new ImgOutput(parent, std::move(name), blueprint);
      }));
}

} // namespace img