#pragma once
#include "img/imgnode.h"
#include "node/param.h"

namespace img {

	
class ImgClear : public img::ImgNode {
public:
	ImgClear(node::Network &parent, util::StringRef name, node::NodeTemplate &tpl);

  void execute(gfx::GraphicsBackend &gfx,
               const ScreenSpaceContext &ctx) override;

  static void registerTemplate();

private:
  node::Output *output_;
  RenderTarget *target_;
};

} // namespace img