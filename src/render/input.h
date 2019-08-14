#pragma once
#include "node.h"
#include <memory>
#include <util/stringref.h>

namespace render {

/// The input of a node that shows up as a connector in the node graph.
class Input {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Input>;

  Input(Node *owner, std::string name) : owner_{owner}, nodeRef_{owner} {
    owner_->insertInput(this);
  }

  ~Input() { owner_->removeInput(this); }

  util::StringRef name() const { return util::StringRef{name_}; }

  static Ptr make(Node *owner, std::string name) {
    return std::make_unique<Input>(owner, std::move(name));
  }

  Node *owner() const { return owner_; }

  void set(std::string node, std::string output) {
	  nodeRef_.set(node, output);
  }

private:
  Node *owner_;
  std::string name_;
  NodeRef nodeRef_;
};

} // namespace render