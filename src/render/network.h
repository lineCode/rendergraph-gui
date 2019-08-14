#pragma once
#include "render/node.h"
#include "util/arrayref.h"

namespace render {

/// A node that contains child nodes.
class Network : public Node {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Network>;

  Network(Network* parent) : Node{ parent } {}
  Network(std::string name, Network* parent) : Node{name, parent} {}
  virtual ~Network() {}

  Node* addChild(Node::Ptr ptr);
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

  void addConnection(util::StringRef from, util::StringRef fromOutput, util::StringRef to, util::StringRef toInput) {
	  auto fromNode = findChildByName(from);
	  if (!fromNode) // emit an error somehow
		  return;
	  auto toNode = findChildByName(to);
	  if (!toNode)
		  return;
	  auto output = fromNode->output(fromOutput);
	  if (!output) return;
	  auto input = toNode->input(toInput);
	  if (!input) return;

	  // found everything
  }

  static Ptr make(Network* parent, std::string name) {
	  return std::make_unique<Network>(std::move(name),parent);
  }

private:
  std::vector<Node::Ptr> children_;
};

} // namespace render