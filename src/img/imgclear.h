#pragma once
#include "img/imgnode.h"

namespace img {

	
class ImgClear : public img::ImgNode {
public:
  ImgClear(node::Network &parent, std::string name, node::Blueprint &blueprint)
      : ImgNode{parent, std::move(name), blueprint} {  

  }

  void execute(gfx::GraphicsBackend &gfx,
               const ScreenSpaceContext &ctx) override;

  static void registerBlueprint();

private:
  node::Output *output_;
  RenderTarget *target_;
};

} // namespace img