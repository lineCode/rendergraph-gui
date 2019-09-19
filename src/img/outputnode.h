#pragma once
#include "img/imgnetwork.h"
#include "img/imgnode.h"

namespace img {
class ImgOutput : public ImgNode {
public:
  ImgOutput(node::Network &parent, util::StringRef name,
            node::NodeTemplate &blueprint);

  void execute(gfx::GraphicsBackend &    gfx,
               const ScreenSpaceContext &ctx) override;

  static void registerTemplate();

private:
  node::Input *result_;
};
} // namespace img