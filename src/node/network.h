#pragma once
#include "node/node.h"
#include "util/arrayref.h"
#include "util/stringref.h"
#include <vector>

namespace node {

class Blueprint;
class BlueprintTable;

/// A node that contains child nodes.
class Network : public Node {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Network>;

  //  Network(Network *parent) : Node{parent} {}
  Network(node::Network *parent, std::string name,
          node::Blueprint *thisBlueprint, BlueprintTable &blueprintTable)
      : Node{parent, name, thisBlueprint}, blueprints_{blueprintTable} {}
  virtual ~Network() {}

  /// Creates a node of the specified type, and adds it to the network.
  Node *createNode(util::StringRef typeName, std::string name);

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

  BlueprintTable &blueprints() const { return blueprints_; }

protected:
  void loadInternal(util::StringRef key, util::JsonReader &reader) override;
  void saveInternal(util::JsonWriter &writer) override;

private:
  void makeNameUnique(std::string &name);

  BlueprintTable &blueprints_;
  std::vector<Node::Ptr> children_;
  int uniqueNameCounter_ = 0;
};

} // namespace node
