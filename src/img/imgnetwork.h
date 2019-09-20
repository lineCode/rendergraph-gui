#pragma once
#include "gfx/gfx.h"
#include "img/rendertarget.h"
#include "node/description.h"
#include "node/network.h"
#include "node/node.h"

#include <unordered_map>

namespace img {

enum class ImageOutputScale {
	/// output size is size defined in the current project
	ProjectSize,
	/// output size is a fraction of the size defined in the current stylization
	/// project
	FractionOfProjectSize,
	/// output size is the size of the first input
	SizeOfFirstInput,
	/// output size is a fraction of the first input
	FractionOfFirstInput,
	/// custom output size
	CustomSize,
	/// output size is input size rescaled to the given aspect ratio
	CustomAspectRatio,
};


class ImgTemplate;
class ImgNode;
class ImgNetwork;

class ImgNetwork : public node::Network {
  friend class ImgNode;
  friend class ImgOutput;

public:
  using Ptr = std::unique_ptr<ImgNetwork>;
  ImgNetwork(util::StringRef name);

  void  onChildAdded(Node *node) override;
  void  onChildRemoved(Node *node) override;
  Node *createNode(util::StringRef typeName, util::StringRef name) override;

  /// Registers a template for creating an IMG node.
  static void registerChild(util::StringRef name, util::StringRef friendlyName,
                            util::StringRef   description,
                            node::Constructor constructor);

  node::NodeDescriptions &registeredNodes() const override {
    return descriptions_;
  }

private:
  void setOutput(ImgOutput *output);

  static node::NodeDescriptions descriptions_;
  ImgOutput *                   output_ = nullptr;
};

} // namespace img