#pragma once
#include "screenspacenetwork.h"

namespace img {
class OutputNode : public node::Node {
public:
  OutputNode(ImgNetwork &parent, std::string name)
      : Node{std::move(name), &parent}, parent_{parent} {
    result_ = createInput("Result");
  }

  void execute(gfx::GraphicsBackend *gfx);

  static OutputNode *OutputNode::make(ImgNetwork &parent, std::string name);

private:
  // The image to display
  ImgNetwork &parent_;
  node::Input *result_;
};
} // namespace img