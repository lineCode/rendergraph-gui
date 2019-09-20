#pragma once
#include "img/imgnetwork.h"
#include "img/imgnode.h"

namespace img {
class ImgOutput : public ImgNode {
public:
  ImgOutput(node::Network &parent, util::StringRef name);

  void execute(ImgContext& ctx) override;
  void prepare(ImgContext& ctx) override;

  static void registerNode();

private:
  node::Input *result_;
};
} // namespace img