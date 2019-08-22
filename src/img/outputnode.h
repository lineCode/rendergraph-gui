#pragma once
#include "img/imgnetwork.h"
#include "img/imgnode.h"

namespace img {
class ImgOutput : public ImgNode {
public:
  ImgOutput(node::Network &parent, std::string name, node::Blueprint& blueprint)
      : ImgNode{ parent, std::move(name), blueprint} {
    result_ = createInput("Result");
  }

  void execute(gfx::GraphicsBackend &gfx,
	  const ScreenSpaceContext &ctx) override;

  static node::Node *make(node::Network &parent, std::string name, node::Blueprint& blueprint);

private:
  node::Input *result_;
};
} // namespace img