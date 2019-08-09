#pragma once
#include "gfx/gfx.h"
#include <map>
#include <string>
#include <vector>

namespace render {

class Node;
class Param;

/// Base class for all nodes.
///
/// The main purpose of this class is to unify the way dependencies between
/// operations are handled.
class Node {
public:
  using Ptr = std::unique_ptr<Node>;

  Node() {}
  Node(std::string name) : name_{name} {}
  virtual ~Node() {}

  // Name
  util::StringRef name() const;
  void setName(std::string name);

  /// Marks this node as dirty.
  void markDirty();

  /// Returns the parent of this node.
  Node *parent() const;

  /// Adds a child node
  Node *addChild(Ptr ptr);

  /// Adds a parameter to this node. This will show up in the user interface.
  Param *addParam(std::unique_ptr<Param> param);

  /// Returns a vector containing all child nodes of the specified type
  template <typename T,
            typename = std::enable_if_t<std::is_base_of<Node, T>::value>>
  std::vector<T *> findChildrenByType() {
    std::vector<T *> results;
    for (auto &&child : children_) {
      auto pchild = child.get();
      if (auto derived = dynamic_cast<T *>(pchild)) {
        results.push_back(derived);
      }
    }
    return results;
  }

private:
  bool dirty_ = true;
  std::string name_;
  Node *parent_ =
      nullptr; // parent of this node, or nullptr if this is the root node
  std::vector<Ptr> children_; // nodes contained inside this node
  std::map<std::string, Param *>
      params_; // node parameters (they are stored as child parameters)
  std::vector<Node *> references_; // nodes referenced by this node (which nodes
                                   // this node depends on?)
  std::vector<Node *> dependents_; // nodes that depend on this node
};


/// Parameters are nodes that are displayed in the
/// parameter panel.
class Param : public Node {
public:
  Param(std::string name, std::string description, double initValue)
      : Node{std::move(name)}, val_{initValue}, description_{
                                                    std::move(description_)} {}

  double value() const { return val_; }

  void setValue(double newValue) { val_ = newValue; }

  static Param *make(Node *parent, std::string name, std::string description,
                     double initValue) {
    auto p = std::make_unique<Param>(std::move(name), std::move(description),
                                     std::move(initValue));
    return static_cast<Param *>(parent->addChild(std::move(p)));
  }

  util::StringRef description() const { return description_; }

private:
  // TODO support more things
  double val_;
  std::string description_;
};

} // namespace render
