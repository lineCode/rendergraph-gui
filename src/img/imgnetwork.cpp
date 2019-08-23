#include "img/imgnetwork.h"
#include "gfx/gfx.h"
#include "img/imgnode.h"
#include "img/outputnode.h"
#include "img/rendertarget.h"
#include "node/blueprint.h"
#include "util/log.h"

#include <unordered_map>

using node::Network;

namespace img {

node::BlueprintTable ImgNetwork::imgBlueprints_;

void ImgNetwork::registerChild(node::Blueprint* blueprint) {
  imgBlueprints_.registerBlueprint(blueprint);
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

} // namespace img