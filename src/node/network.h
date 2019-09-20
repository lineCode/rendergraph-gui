#pragma once
#include "node/node.h"
#include "util/arrayref.h"
#include "util/stringref.h"
#include <vector>

namespace node {

class NodeDescription;
class NodeDescriptions;

/// A node that contains child nodes.
class Network : public Node {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Network>;

  //  Network(Network *parent) : Node{parent} {}
  Network(node::Network *parent, util::StringRef name)
      : Node{parent, name } {}
  virtual ~Network() {}

  /// Creates a node of the specified type, and adds it to the network.
  virtual Node *createNode(util::StringRef typeName, util::StringRef name) = 0;

  /// Adds a child node to this network. The network will take ownership of `node`. 
  /// `node ` must have been allocated with `new`.
  Node *addChild(Node *node);

  /// Deletes a child node.
  void deleteChild(Node *node);
  void deleteChildren(util::ArrayRef<Node *const> nodes);

  /// Returns a vector containing all child nodes of the specified type.
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

  ///
  std::vector<std::vector<Node*>> buildAdjacencyList();

  /// Returns a vector containing a topologically sorted list of all the child nodes in this network.
  std::vector<Node*> sortedChildren();

  ///
  virtual NodeDescriptions& registeredNodes() const = 0;

protected:
  void loadInternal(util::StringRef key, util::JsonReader &reader) override;
  void saveInternal(util::JsonWriter &writer) override;

private:
  void makeNameUnique(std::string &name);

  std::vector<Node::Ptr> children_;
  int uniqueNameCounter_ = 0;
};

} // namespace node
