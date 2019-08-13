#pragma once
#include "render/node.h"
#include <memory>
#include <string>

namespace render {

/// The output of a node that shows up as a connector in the node graph.
class Output {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Output>;

  Output(Node *owner, std::string name) : owner_{owner} {
    owner_->insertOutput(this);
  }

  ~Output() { owner_->removeOutput(this); }

  util::StringRef name() const { return util::StringRef{name_}; }

  static Ptr make(Node *owner, std::string name) {
    return std::make_unique<Output>(owner, std::move(name));
  }

private:
  Node *owner_;
  std::string name_;
};

} // namespace render