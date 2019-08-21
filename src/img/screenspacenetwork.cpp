#include "img/screenspacenetwork.h"
#include "gfx/gfx.h"
#include "img/rendertarget.h"
#include "img/screenspacenode.h"
#include "img/outputnode.h"
#include "util/log.h"

#include <unordered_map>

using node::Network;

namespace img {

void ImgNetwork::setOutput(OutputNode *output) {
  output_ = output;
  util::log("ImgNetwork[{}]: setting output node -> {}",
            name().to_string(), output->name().to_string());
}

void ImgNetwork::onChildAdded(Node *node) {
  Node::onChildAdded(node);
  if (auto out = dynamic_cast<OutputNode *>(node)) {
    if (!output_) {
      // it's an output node, and there are no registered outputs yet
      // choose this one
      setOutput(out);
    }
  }
}

void ImgNetwork::onChildRemoved(Node *node) {
  if (output_ == node) {
    // output node was removed
    setOutput(nullptr);
  }
}

} // namespace img