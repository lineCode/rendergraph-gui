#include "render/node.h"
#include <algorithm>

namespace render {
Node::~Node() {
  // this node is being deleted, signal observers
  onNodeDeleted();
}

util::StringRef Node::name() const {
  return util::StringRef{name_.c_str(), name_.size()};
}

void Node::setName(std::string name) { name_ = std::move(name); }

void Node::markDirty() {
  dirty_ = true;
  // TODO: propagate to children
}

Node *Node::parent() const { return parent_; }

Node *Node::addChild(Ptr ptr) {
  auto p = ptr.get();
  p->parent_ = this;
  children_.push_back(std::move(ptr));
  return p;
}

void Node::deleteNode(const Node *node) {
  const Node *nodes[1] = {node};
  deleteNodes(util::makeConstArrayRef(nodes));
}

void Node::deleteNodes(util::ArrayRef<const Node *const> nodes) {
  auto it = std::remove_if(children_.begin(), children_.end(),
                           [nodes](const std::unique_ptr<Node> &ptr) {
                             for (auto n : nodes) {
                               if (ptr.get() == n) {
                                 return true;
                               }
                             }
                             return false;
                           });
  children_.erase(it, children_.end());
}

void Node::disconnect() const {
  for (auto &&p : parent_->children_) {
  }
}

// child added
void Node::onChildAdded(const Node *node) {
	// TODO
}

// child about to be removed
void Node::onChildRemoved(const Node *node) {
	// TODO
}

void Node::onReferenceAdded(const Node *to) {
	// TODO
}

void Node::onReferenceRemoved(const Node *to) {
	// TODO
}

void Node::onNodeDeleted() {
	EventData e;
	e.source = this;
	e.type = EventType::NodeDeleted;
	notify(e);
}

void Node::notify(const EventData &e) {
  notifying_++;
  for (auto &&obs : observers_) {
    notifying_ = true;
    obs->callback_(e);
  }
  notifying_--;
  if (notifying_ == 0) {
    auto it = std::remove_if(observers_.begin(), observers_.end(),
                             [this](Observer *ptr) {
                               for (auto obs : this->observersToRemove_) {
                                 if (ptr == obs) {
                                   return true;
                                 }
                               }
                               return false;
                             });
    observers_.erase(it, observers_.end());
    observers_.insert(observers_.end(), observersToAdd_.begin(),
                      observersToAdd_.end());
  }
}

} // namespace render