#pragma once
#include "gfx/gfx.h"
#include "img/rendertarget.h"
#include "node/network.h"
#include "node/node.h"

namespace img {

struct ScreenSpaceContext {
  /// Width of the viewport in pixels
  int width;
  /// Height of the viewport in pixels
  int height;
  /// Current time
  double currentTime;
  /// Current frame
  int currentFrame;
  /// Constant buffer containing the common parameters (camera, matrices, etc.)
  gfx::ConstantBufferView commonParameters;
  gfx::VertexBufferView quadVertices;
};

class ImgNetwork : public node::Network {
  friend class ImgNode;
  friend class ImgOutput;

public:
  using Ptr = std::unique_ptr<ImgNetwork>;

  ImgNetwork(node::Network *parent, std::string name)
      : Network{parent, name, nullptr, imgBlueprints_} {}

  void onChildAdded(Node *node) override;
  void onChildRemoved(Node *node) override;

  static void registerChild(node::Blueprint* blueprint);

private:
  static node::BlueprintTable imgBlueprints_;
  void setOutput(ImgOutput *output);
  ImgOutput *output_ = nullptr;
};

} // namespace img