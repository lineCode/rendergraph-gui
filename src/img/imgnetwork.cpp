#include "img/imgnetwork.h"
#include "gfx/gfx.h"
#include "img/imgnode.h"
#include "img/outputnode.h"
#include "img/rendertarget.h"
#include "node/template.h"
#include "util/log.h"

#include <unordered_map>

using node::Network;
using node::Node;
using node::NodeTemplate;

namespace img {

void ImgNetwork::registerTemplate(
    util::StringRef name, util::StringRef friendlyName,
    util::StringRef description, util::StringRef icon,
    util::ArrayRef<const node::ParamDesc*>  params,
    util::ArrayRef<const node::InputDesc *>  inputs,
    util::ArrayRef<const node::OutputDesc *> outputs,
    node::Constructor                        constructor) {
  // TODO
  auto tpl = new node::NodeTemplate(name.to_string(), friendlyName.to_string(),
                                    description.to_string(), icon.to_string(),
                                    params, inputs, outputs, constructor);
  imgTemplates_.registerTemplate(std::unique_ptr<NodeTemplate>(tpl));
}

static Node *createImgNetwork(Network &parent, util::StringRef name,
                              NodeTemplate &tpl) {
  return new ImgNetwork(name);
}

NodeTemplate &ImgNetwork::getTemplate() {
  static NodeTemplate tpl{"img",   "IMG network", "IMG network",   "", nullptr,
                          nullptr, nullptr,       createImgNetwork};
  return tpl;
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

ImgNetwork::ImgNetwork(util::StringRef name)
    : Network{nullptr, name, getTemplate()} {}

Node *ImgNetwork::createNode(util::StringRef typeName, util::StringRef name) {
  auto bp = imgTemplates_.findTemplate(typeName);
  if (!bp) {
    util::log("WARNING ImgNetwork::createNode: unknown node type `{}`",
              typeName.to_string());
  }
  return addChild(bp->make(*this, std::move(name)));
}

node::TemplateTable ImgNetwork::imgTemplates_;

} // namespace img