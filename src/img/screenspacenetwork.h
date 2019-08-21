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
  friend class OutputNode;

public:

	using Ptr = std::unique_ptr<ImgNetwork>;

  ImgNetwork(Network *parent, std::string name,
                     RenderTargetCache &renderTargetCache)
      : Network{std::move(name), parent}, renderTargetCache_{
                                              renderTargetCache} {}

  ImgNetwork *
  ImgNetwork::make(Network *parent, std::string name,
                           RenderTargetCache &renderTargetCache) {
    return static_cast<ImgNetwork *>(
        parent->addChild(std::make_unique<ImgNetwork>(
            parent, std::move(name), renderTargetCache)));
  }

  void onChildAdded(Node *node) override;

  void onChildRemoved(Node *node) override;

private:
  void setOutput(OutputNode *output);
  RenderTargetCache &renderTargetCache_;
  OutputNode *output_ = nullptr;
};

} // namespace img