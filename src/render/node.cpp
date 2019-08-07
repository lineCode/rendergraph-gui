#include "render/node.h"

namespace render {

util::StringRef Node::name() const {
  return util::StringRef{name_.c_str(), name_.size()};
}

void Node::setName(std::string name) { name_ = std::move(name); }

Node *Node::parent() const { return parent_; }

Node *Node::addChild(Ptr ptr) {
  auto p = ptr.get();
  children_.push_back(std::move(ptr));
  return p;
}

} // namespace render