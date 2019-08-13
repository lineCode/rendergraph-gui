#include "render/node.h"
#include "render/network.h"
#include "render/observer.h"
#include "util/log.h"
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

Network *Node::parent() const { return parent_; }

int Node::paramCount() const {
	return params_.size();
}

Param * Node::param(int index)
{
	if (index >= params_.size()) {
		return nullptr;
	}
	return params_[index];
}

void Node::disconnect() const {
  for (auto &&p : parent_->children_) {
    // TODO
  }
}

// child added
void Node::onChildAdded(Node *node) {
  // TODO
  util::log("Node[{}]::onChildAdded({})", name().to_string(),
            node->name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::ChildAdded;
  e.u.childAdded.node = node;
  notify(e);
}

// child about to be removed
void Node::onChildRemoved(Node *node) {

  util::log("Node[{}]::onChildRemoved({})", name().to_string(),
            node->name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::ChildRemoved;
  e.u.childRemoved.node = node;
  notify(e);
}

void Node::onReferenceAdded(Node *to) {
  util::log("Node[{}]::onReferenceAdded({})", name().to_string(),
            to->name().to_string());
  // TODO
}

void Node::onReferenceRemoved(Node *to) {
  util::log("Node[{}]::onReferenceRemoved({})", name().to_string(),
            to->name().to_string());
  // TODO
}

void Node::onNodeDeleted() {
  util::log("Node[{}]::onNodeDeleted()", name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::NodeDeleted;
  notify(e);
}

void Node::onInputAdded(int index) {
  util::log("Node[{}]::onInputAdded({})", name().to_string(), index);
  EventData e;
  e.source = this;
  e.type = EventType::InputAdded;
  e.u.inputAdded.index = index;
  notify(e);
}

void Node::onInputRemoved(int index) {
  util::log("Node[{}]::onInputRemoved({})", name().to_string(), index);
  EventData e;
  e.source = this;
  e.type = EventType::InputRemoved;
  e.u.inputRemoved.index = index;
  notify(e);
}

void Node::onOutputAdded(int index) {
  // TODO
}

void Node::onOutputRemoved(int index) {
  // TODO
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

void Node::addReference(NodeRef *ref) { references_.push_back(ref); }

void Node::removeReference(NodeRef *ref) {
  auto it = std::remove(references_.begin(), references_.end(), ref);
  references_.erase(it, references_.end());
}

void Node::addParameter(Param *p) { params_.push_back(p); }

void Node::removeParameter(Param *p) {
  auto it = std::remove(params_.begin(), params_.end(), p);
  params_.erase(it, params_.end());
}

void Node::addObserver(Observer *obs) {
  if (notifying_) {
    observersToAdd_.push_back(obs);
  } else {
    observers_.push_back(obs);
  }
}

void Node::removeObserver(Observer *obs) {
  if (notifying_) {
    observersToRemove_.push_back(obs);
  } else {
    auto it = std::remove(observers_.begin(), observers_.end(), obs);
    observers_.erase(it, observers_.end());
  }
}

void Node::insertInput(Input *input, int index) {
  if (index == -1) {
    inputs_.push_back(input);
    onInputAdded(inputs_.size() - 1);
  } else {
    inputs_.insert(inputs_.begin() + index, input);
    onInputAdded(index);
  }
}

void Node::removeInput(Input *input) {
  auto it = std::remove(inputs_.begin(), inputs_.end(), input);
  int index = it - inputs_.begin();
  if (it != inputs_.end()) {
    inputs_.erase(it, inputs_.end());
    onInputRemoved(index);
  }
}

void Node::insertOutput(Output *output, int index) {
  if (index == -1) {
    outputs_.push_back(output);
    onOutputAdded(outputs_.size() - 1);
  } else {
    outputs_.insert(outputs_.begin() + index, output);
    onOutputAdded(index);
  }
}

void Node::removeOutput(Output *output) {
  auto it = std::remove(outputs_.begin(), outputs_.end(), output);
  int index = it - outputs_.begin();
  if (it != outputs_.end()) {
    outputs_.erase(it, outputs_.end());
    onOutputRemoved(index);
  }
}

} // namespace render