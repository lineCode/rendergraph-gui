
#include "network.h"
#include <algorithm>

namespace render {

Node *Network::addChild(Node::Ptr ptr) {
  auto p = ptr.get();
  children_.push_back(std::move(ptr));
  onChildAdded(p);
  return p;
}

void Network::deleteChild(Node *node) {
  Node *nodes[1] = {node};
  deleteChildren(util::makeConstArrayRef(nodes));
}

void Network::deleteChildren(util::ArrayRef<Node *const> nodes) {
  auto it = std::remove_if(children_.begin(), children_.end(),
                           [this,nodes](const std::unique_ptr<Node> &ptr) {
                             for (auto n : nodes) {
                               if (ptr.get() == n) {
								   onChildRemoved(n);
                                 return true;
                               }
                             }
                             return false;
                           });
  //for (auto it2 = it; it2 < children_.end(); ++it2) {
  //}
  children_.erase(it, children_.end());
}

Node *Network::findChildByName(util::StringRef name) {
  auto p = std::find_if(children_.begin(), children_.end(),
                        [name](const Node::Ptr &n) {
                          if (n->name() == name) {
                            return true;
                          }
                          return false;
                        });
  if (p != children_.end()) {
    return (*p).get();
  }
  return nullptr;
}

} // namespace render