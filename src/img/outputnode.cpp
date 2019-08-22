#include "img/outputnode.h"

namespace img {

void ImgOutput::execute(gfx::GraphicsBackend &gfx, const ScreenSpaceContext &ctx) {
  // resolve image
  // gfx->presentToScreen()
}

node::Node *ImgOutput::make(node::Network &parent, std::string name, node::Blueprint& blueprint) {
	parent.addChild(std::make_unique<ImgOutput>(parent, std::move(name), blueprint));
}

} // namespace img