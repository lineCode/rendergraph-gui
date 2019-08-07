#pragma once
#include "gfx/gfx.h"
#include <string>
#include <vector>

namespace render {

/// Base class for all nodes.
///
/// The main purpose of this class is to unify the way dependencies between
/// operations are handled.
class Node {
public:
  using Ptr = std::unique_ptr<Node>;

  Node() {}
  Node(std::string name) : name_{ name }
  {}

  // Name
  util::StringRef name() const;
  void setName(std::string name);

  /// Returns the parent of this node.
  Node *parent() const;

  /// Adds a child node
  Node *addChild(Ptr ptr);

private:
	std::string name_;
  Node *parent_ = nullptr; // parent of this node, or nullptr if this is the root node
  std::vector<Ptr> children_;      // nodes contained inside this node
  std::vector<Node *> references_; // nodes referenced by this node (which nodes
                                   // this node depends on?)
  std::vector<Node *> dependents_; // nodes that depend on this node
};

} // namespace render
