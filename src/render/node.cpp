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

int Node::paramCount() const { return params_.size(); }

Param *Node::param(int index) {
  if (index >= params_.size()) {
    return nullptr;
  }
  return params_[index].get();
}

void Node::disconnect() {
  for (auto &&p : parent_->children_) {
    // TODO
  }
}

Input *Node::createInput(std::string name) {
  auto input =
      pushUniquePtr(inputs_, Input::make(this, getInputId(), std::move(name)));
  onInputAdded(input);
  return input;
}

void Node::deleteInput(Input *input) {
  onInputRemoved(input);
  eraseRemoveUniquePtr(inputs_, input);
}

Output *Node::createOutput(std::string name) {
  auto output =
      pushUniquePtr(outputs_, Output::make(getOutputId(), std::move(name)));
  onOutputAdded(output);
  return output;
}

void Node::deleteOutput(Output *output) {
  onOutputRemoved(output);
  // notify dependents
  for (auto dep : output->dependents_) {
    // unlink all references on all plugged nodes
    dep->referenceDeleted(this, output);
  }
  eraseRemoveUniquePtr(outputs_, output);
}

Param *Node::createParameter(std::string name, std::string description,
                             double initValue) {
  auto param =
      pushUniquePtr(params_, Param::make(this, std::move(name),
                                         std::move(description), initValue));
  return param;
}

void Node::deleteParameter(Param *p) { eraseRemoveUniquePtr(params_, p); }

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

void Node::onConnectOutput(Node *node, Output *output) {

  util::log("Node[{}]::onPlugOutput({})", name().to_string(),
            output->name().to_string(), node->name().to_string());
  // Nothing yet
}

void Node::onDisconnectOutput(Node *node, Output *output) {

  util::log("Node[{}]::onPlugOutput({})", name().to_string(),
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

void Node::onOutputRemoved(Output *output) {
  util::log("Node[{}]::onOutputRemoved({})", name().to_string(),
            output->name().to_string());

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

void Node::referenceDeleted(Node *ref, Output *output) {
  for (auto r : references_) {
    if (r->target_ == ref && r->output_ == output) {
      r->referenceDeleted();
    }
  }
}

void Node::addReference(NodeRef *ref) { references_.push_back(ref); }

void Node::removeReference(NodeRef *ref) {
  auto it = std::remove(references_.begin(), references_.end(), ref);
  references_.erase(it, references_.end());
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

Input *Node::input(int id) {
  for (auto &&i : inputs_) {
    if (i->id() == id) {
      return i.get();
    }
  }
  return nullptr;
}

Input *Node::input(util::StringRef name) {
  for (auto &&i : inputs_) {
    if (i->name() == name) {
      return i.get();
    }
  }
  return nullptr;
}

Output *Node::output(int id) {
  for (auto &&o : outputs_) {
    if (o->id() == id) {
      return o.get();
    }
  }
  return nullptr;
}

Output *Node::output(util::StringRef name) {
  for (auto &&o : outputs_) {
    if (o->name() == name) {
      return o.get();
    }
  }
  return nullptr;
}

void Node::connectInput(Input *input, Node *source, Output *output) {
  input->set(source->name().to_string(), output->name().to_string());
}

void Node::connectOutput(Node *dep, Output *output) {
  onConnectOutput(dep, output);
  output->addDependent(dep);
}

void Node::disconnectOutput(Node *dep, Output *output) {
  onDisconnectOutput(dep, output);
  output->removeDependent(dep);
}

NodeRef::NodeRef(Node *owner, std::string path, std::string output)
    : owner_{owner}, path_{std::move(path)} {
  owner_->addReference(this);
  resolve();
}

NodeRef::~NodeRef() { owner_->removeReference(this); }

bool NodeRef::resolve() {
  if (target_)
    return true;
  auto node = owner_->parent()->findChildByName(path_);
  if (!node)
    return false;
  auto output_ = node->output(outputName_);
  if (!output_)
    return false;
  // link the nodes
  target_ = node;
  target_->connectOutput(owner_, output_);
}

void NodeRef::disconnect() {
  if (!target_)
    return;
  target_->disconnectOutput(owner_, output_);
}

void NodeRef::referenceDeleted() {
  target_ = nullptr;
  // TODO set error on the owner node
}

} // namespace render