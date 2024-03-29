#pragma once
#include "img/imgnode.h"
#include "node/param.h"

namespace img {

	
class ImgClear : public img::ImgNode {
public:
	ImgClear(node::Network &parent, util::StringRef name);

  void execute(ImgContext& ctx) override;
  void prepare(ImgContext& ctx) override;

  static void registerNode();

private:
  //node::Output *output_;
  //RenderTarget *target_;
};

} // namespace img