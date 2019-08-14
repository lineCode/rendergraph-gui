#pragma once
#include "render/node.h"
#include <memory>
#include <string>

namespace render {

/// The output of a node that shows up as a connector in the node graph.
class Output {
  friend class Node;
  friend class NodeRef;

public:
  using Ptr = std::unique_ptr<Output>;

  Output(Node *owner, std::string name) : owner_{owner} {
    owner_->insertOutput(this);
  }

  ~Output() { owner_->removeOutput(this); }

  Node *owner() const { return owner_; }

  util::StringRef name() const { return util::StringRef{name_}; }

  static Ptr make(Node *owner, std::string name) {
    return std::make_unique<Output>(owner, std::move(name));
  }

  void addDependent(Node *node) {
    // notify parent node that a dependency has been added to this output
    owner_->onDependentAdded(this, node);
    dependents_.push_back(node);
  }

  void removeDependent(Node *node) {
    // notify parent node that a dependency is about to be removed
    owner_->onDependentRemoved(this, node);
    auto it = std::remove(dependents_.begin(), dependents_.end(), node);
    dependents_.erase(it, dependents_.end());
  }

private:
  Node *owner_;
  std::string name_;
  std::vector<Node *> dependents_; // dependent nodes
};

} // namespace render