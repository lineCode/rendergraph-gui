#include "render/node.h"
#include "render/input.h"
#include "render/network.h"
#include "render/observer.h"
#include "render/output.h"
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

int Node::paramCount() const { return params_.size(); }

Param *Node::param(int index) {
  if (index >= params_.size()) {
    return nullptr;
  }
  return params_[index];
}

void Node::disconnect() {
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

void Node::onDependentAdded(Output *output, Node *node) {

  util::log("Node[{}]::onDependentAdded({})", name().to_string(),
            output->name().to_string(), node->name().to_string());
  // Nothing yet
}

void Node::onDependentRemoved(Output *output, Node *node) {

  util::log("Node[{}]::onDependentRemoved({})", name().to_string(),
            output->name().to_string(), node->name().to_string());
  // Nothing yet
}

void Node::onNodeDeleted() {
  util::log("Node[{}]::onNodeDeleted()", name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::NodeDeleted;
  notify(e);
}

void Node::onInputAdded(Input *input) {
  util::log("Node[{}]::onInputAdded({})", name().to_string(),
            input->name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::InputAdded;
  e.u.inputAdded.input = input;
  notify(e);
}

void Node::onInputRemoved(Input *input) {
  util::log("Node[{}]::onInputRemoved({})", name().to_string(),
            input->name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::InputRemoved;
  e.u.inputRemoved.input = input;
  notify(e);
}

void Node::onOutputAdded(Output *output) {
  util::log("Node[{}]::onOutputAdded({})", name().to_string(),
            output->name().to_string());
  EventData e;
  e.source = this;
  e.type = EventType::OutputAdded;
  e.u.outputAdded.output = output;
  notify(e);
}

// The output of a referenced node is about to be removed,
// unlink all references to this output.
void Node::referencedOutputDeleted(Output *output) {
  for (auto ref : references_) {
    if (ref->target_ == output) {
      ref->unlink();
    }
  }
}

void Node::onOutputRemoved(Output *output) {
  util::log("Node[{}]::onOutputRemoved({})", name().to_string(),
            output->name().to_string());
  // notify dependents
  for (auto dep : output->dependents_) {
    dep->referencedOutputDeleted(output);
  }

  EventData e;
  e.source = this;
  e.type = EventType::OutputRemoved;
  e.u.outputRemoved.output = output;
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
    onInputAdded(input);
  } else {
    inputs_.insert(inputs_.begin() + index, input);
    onInputAdded(input);
  }
}

void Node::removeInput(Input *input) {
  onInputRemoved(input);
  auto it = std::remove(inputs_.begin(), inputs_.end(), input);
  int index = it - inputs_.begin();
  if (it != inputs_.end()) {
    inputs_.erase(it, inputs_.end());
  }
}

void Node::insertOutput(Output *output, int index) {
  if (index == -1) {
    outputs_.push_back(output);
    onOutputAdded(output);
  } else {
    outputs_.insert(outputs_.begin() + index, output);
    onOutputAdded(output);
  }
}

void Node::removeOutput(Output *output) {
  onOutputRemoved(output);
  auto it = std::remove(outputs_.begin(), outputs_.end(), output);
  int index = it - outputs_.begin();
  if (it != outputs_.end()) {
    outputs_.erase(it, outputs_.end());
  }
}

Input *Node::input(int index) { return inputs_[index]; }

Input *Node::input(util::StringRef name) {
  for (auto i : inputs_) {
    if (i->name() == name) {
      return i;
    }
  }
  return nullptr;
}

Output *Node::output(int index) { return outputs_[index]; }

Output *Node::output(util::StringRef name) {
  for (auto o : outputs_) {
    if (o->name() == name) {
      return o;
    }
  }
  return nullptr;
}

NodeRef::NodeRef(Node *owner, std::string path, std::string output)
    : owner_{owner}, path_{std::move(path)} {
  owner_->addReference(this);
  resolve();
}

NodeRef::~NodeRef() { owner_->removeReference(this); }

Output *NodeRef::resolve() {
  if (target_)
    return target_;
  auto node = owner_->parent()->findChildByName(path_);
  if (!node)
    return nullptr;
  auto output = node->output(output_);
  if (!output)
    return nullptr;
  // link the nodes
  output->addDependent(owner_);
}

void NodeRef::unlink() {
  if (target_) {
	
  }

  // TODO set error on the owner node
}


} // namespace render