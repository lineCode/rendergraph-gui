#include "img/imgnetwork.h"
#include "gfx/gfx.h"
#include "img/imgnode.h"
#include "img/imgoutput.h"
#include "img/rendertarget.h"
#include "node/description.h"
#include "util/log.h"

#include <unordered_map>

using node::Network;
using node::Node;
using node::NodeDescription;

namespace img {

void ImgNetwork::registerChild(util::StringRef   name,
                              util::StringRef   friendlyName,
                              util::StringRef   description,
                              node::Constructor constructor) {
  descriptions_.registerNode(name.to_string(), friendlyName.to_string(),
                             description.to_string(), constructor);
}

static Node *constructor(Network &parent, util::StringRef name) {
  return new ImgNetwork(name);
}

void ImgNetwork::setOutput(ImgOutput *output) {
  if (output) {
    util::log("ImgNetwork[{}]: setting output node -> {}", name().to_string(),
              output->name().to_string());
  } else {
    util::log("ImgNetwork[{}]: unsetting output", name().to_string());
  }
  output_ = output;
}

void ImgNetwork::onChildAdded(Node *node) {
  Network::onChildAdded(node);
  if (auto out = dynamic_cast<ImgOutput *>(node)) {
    if (!output_) {
      // it's an output node, and there are no registered outputs yet
      // choose this one
      setOutput(out);
    }
  }
}

void ImgNetwork::onChildRemoved(Node *node) {
  Network::onChildRemoved(node);
  if (output_ == node) {
    // output node was removed
    setOutput(nullptr);
  }
}

ImgNetwork::ImgNetwork(util::StringRef name) : Network{nullptr, name} {}

Node *ImgNetwork::createNode(util::StringRef typeName, util::StringRef name) {
  auto desc = descriptions_.find(typeName);
  if (!desc) {
    util::log("WARNING ImgNetwork::createNode: unknown node type `{}`",
              typeName.to_string());
  }
  return addChild(desc->instantiate(*this, name));
}

node::NodeDescriptions ImgNetwork::descriptions_;

} // namespace img