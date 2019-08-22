#pragma once
#include "node/node.h"
#include "util/arrayref.h"

namespace node {

/// A node that contains child nodes.
class Network : public Node {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Network>;

  //  Network(Network *parent) : Node{parent} {}
  Network(node::Network &parent, std::string name, node::Blueprint& blueprint) : Node{&parent, name, blueprint} {}
  virtual ~Network() {}

  Node *addChild(Node::Ptr ptr);
  void deleteChild(Node *node);
  void deleteChildren(util::ArrayRef<Node *const> nodes);

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

  Node *findChildByName(util::StringRef name);

  void addConnection(util::StringRef from, util::StringRef fromOutput,
                     util::StringRef to, util::StringRef toInput);

  void addConnection(Node *source, Output *output, Node *destination,
                     Input *input);

protected:
  void loadInternal(util::StringRef key, util::JsonReader &reader) override;
  void saveInternal(util::JsonWriter &writer) override;

private:
  void makeNameUnique(std::string &name);

  std::vector<Node::Ptr> children_;
  int uniqueNameCounter_ = 0;
};

} // namespace node
