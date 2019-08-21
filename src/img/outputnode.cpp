#include "img/outputnode.h"

namespace img {

void OutputNode::execute(gfx::GraphicsBackend *gfx) {
  // resolve image
  // gfx->presentToScreen()
}


OutputNode *OutputNode::make(ImgNetwork &parent, std::string name) {
	return static_cast<OutputNode *>(parent.addChild(
		std::make_unique<OutputNode>(parent, std::move(name))));
}

} // namespace img