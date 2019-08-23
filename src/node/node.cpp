#include "node/node.h"
#include "node/network.h"
#include "node/observer.h"
#include "node/param.h"
#include "util/log.h"
#include <algorithm>

using util::Value;

namespace node {
static int uniqueNodeIdCounter = 0;

static int getUniqueNodeIdCounter() { return uniqueNodeIdCounter++; }

static void setUniqueNodeIdCounter(int value) { uniqueNodeIdCounter = value; }


//------------------------------------------------------------------------------------
Node::Node(node::Network *parent, std::string name, node::Blueprint *blueprint)
    : name_{name}, parent_{parent}, blueprint_{blueprint} {
  id_ = getUniqueNodeIdCounter();
}

Node::~Node() {
  // disconnect all inputs
  for (auto &&input : inputs_) {
    disconnectInput(input.get());
  }
  // disconnect all outputs
  for (auto &&output : outputs_) {
    disconnectOutput(output.get());
  }

  // signal that the node about to be removed from its parent network
  if (parent_)
    parent_->onChildRemoved(this);

  // signal that the node is about to be deleted
  onNodeDeleted();

  // node deleting
}

util::StringRef Node::name() const {
  return util::StringRef{name_.c_str(), name_.size()};
}

void Node::setName(std::string name) { name_ = std::move(name); }

int Node::uniqueId() { return id_; }

void Node::markDirty() {
  dirty_ = true;
  // TODO: propagate to children
}

Network *Node::parent() const { return parent_; }

int Node::paramCount() const { return (int)params_.size(); }

Param *Node::param(int index) {
  if (index >= params_.size()) {
    return nullptr;
  }
  return params_[index].get();
}

std::string Node::makeUniqueInputName(std::string name, int id) {
  for (auto &&c : inputs_) {
    if (c->name == name)
      return fmt::format("{}_{}", name, id);
  }
  return name;
}

std::string Node::makeUniqueOutputName(std::string name, int id) {
  for (auto &&c : outputs_) {
    if (c->name == name)
      return fmt::format("{}_{}", name, id);
  }
  return name;
}

Input *Node::createInput(std::string name) {
  int inputId = getInputId();
  return createInputInternal(name, inputId);
}

Input *Node::createInputInternal(std::string name, int uid) {
  Input i;
  i.id = uid;
  i.name = makeUniqueInputName(name, uid);
  i.showConnector = true;
  auto ptr = pushUniquePtr(inputs_, std::make_unique<Input>(std::move(i)));
  onInputAdded(ptr);
  return ptr;
}

void Node::deleteInput(Input *input) {
  // first, remove all connections on this input
  disconnectInput(input);
  // nothing is connected to the input anymore:
  // signal that the input is about to be removed
  onInputRemoved(input);
  // actually delete the input
  eraseRemoveUniquePtr(inputs_, input);
}

int Node::referenceCount(Node *node, Output *output) {
  int count = 0;
  for (auto &&input : inputs_) {
    if (input->source == node && input->output == output) {
      count++;
    }
  }
  return count;
}

bool Node::resolveInput(Input *input) {
  if (input->source) {
    // skip if already resolved
    return true;
  }
  // find source node
  auto source = parent_->findChildByName(input->sourcePath);
  if (!source)
    return false;
  // find output
  auto output = source->output(input->sourceOutputName);
  if (!output)
    return false;
  // found corresponding source and output
  input->source = source;
  input->output = output;
  return true;
}

void Node::connectInput(Input *input, std::string source, std::string output) {
  // disconnect whatever is cuurently connected
  disconnectInput(input);
  // update path and output name (even if they do not resolve to anything)
  input->sourcePath = std::move(source);
  input->sourceOutputName = std::move(output);
  // try to resolve the node and output
  if (resolveInput(input)) {
    // successful, add this node as a dependent node if it's not added already
    if (referenceCount(input->source, input->output)) {
      input->source->addDependentNode(input->output, this, input);
    }
    // ... and signal observers that a connection has been made
    if (parent_)
      parent_->onConnectionAdded(input->source, input->output, this, input);
  }
}

void Node::connectInput(Input *input, Node *source, Output *output) {
  connectInput(input, source->name().to_string(), output->name);
}

void Node::disconnectInput(Input *input) { doDisconnectInput(input, true); }

/// removeReferenceFromOutput: whether to remove the entry in the source output.
void Node::doDisconnectInput(Input *input, bool removeReferenceFromOutput) {
  // remove all connections involving this input
  if (input->source) {
    // signal that a connection is about to be removed
    if (parent_)
      parent_->onConnectionRemoved(input->source, input->output, this, input);

    if (removeReferenceFromOutput) {
      // input is connected
      if (referenceCount(input->source, input->output) == 1) {
        // this is the only reference to this output in this node, so remove it
        // from the source.
        input->source->removeDependentNode(input->output, this, input);
      }
    }

    input->source = nullptr;
    input->output = nullptr;
  }
}

void Node::addDependentNode(Output *output, Node *destination, Input *input) {
  // TODO send event?
  output->dependents.push_back(destination);
}

void Node::removeDependentNode(Output *output, Node *destination,
                               Input *input) {
  // TODO send event? (output connector update)
  auto it = std::find(output->dependents.begin(), output->dependents.end(),
                      destination);
  output->dependents.erase(it, output->dependents.end());
}

Output *Node::createOutput(std::string name) {
  int outputId = getOutputId();
  return createOutputInternal(std::move(name), outputId);
}

Output *Node::createOutputInternal(std::string name, int uid) {

  int outputId = getOutputId();
  Output out;
  out.id = outputId;
  out.name = makeUniqueOutputName(name, outputId);
  auto ptr = pushUniquePtr(outputs_, std::make_unique<Output>(std::move(out)));
  onOutputAdded(ptr);
  return ptr;
}

void Node::deleteOutput(Output *output) {
  // remove all outgoing connections
  disconnectOutput(output);
  // nothing is connected to the input anymore:
  // signal that the output is about to be removed
  onOutputRemoved(output);
  // actually delete the output
  eraseRemoveUniquePtr(outputs_, output);
}

void Node::disconnectOutput(Output *output) {
  // go through all dependent nodes and remove inputs that reference this output
  for (auto &&d : output->dependents) {
    for (auto &&i : d->inputs_) {
      if (i->source == this && i->output == output) {
        // don't remove the entry in the output since we're iterating on
        // output->dependents and this will invalidate the iterators
        d->doDisconnectInput(i.get(), false);
      }
    }
  }
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

void Node::onConnectOutput(Output *output, Node *destination) {

  util::log("Node[{}]::onConnectOutput({},{})", name().to_string(),
            output->name, destination->name().to_string());
  // Nothing yet
}

void Node::onDisconnectOutput(Output *output, Node *destination) {

  util::log("Node[{}]::onDisconnectOutput({},{})", name().to_string(),
            output->name, destination->name().to_string());
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
  util::log("Node[{}]::onInputAdded({})", name().to_string(), input->name);
  EventData e;
  e.source = this;
  e.type = EventType::InputAdded;
  e.u.inputAdded.input = input;
  notify(e);
}

void Node::onInputRemoved(Input *input) {
  util::log("Node[{}]::onInputRemoved({})", name().to_string(), input->name);
  EventData e;
  e.source = this;
  e.type = EventType::InputRemoved;
  e.u.inputRemoved.input = input;
  notify(e);
}

void Node::onOutputAdded(Output *output) {
  util::log("Node[{}]::onOutputAdded({})", name().to_string(), output->name);
  EventData e;
  e.source = this;
  e.type = EventType::OutputAdded;
  e.u.outputAdded.output = output;
  notify(e);
}

void Node::onOutputRemoved(Output *output) {
  util::log("Node[{}]::onOutputRemoved({})", name().to_string(), output->name);

  EventData e;
  e.source = this;
  e.type = EventType::OutputRemoved;
  e.u.outputRemoved.output = output;
  notify(e);
}

void Node::onConnectionAdded(Node *source, Output *output, Node *dest,
                             Input *input) {
  util::log("Node[{}]::onConnectionAdded({},{},{},{})", name().to_string(),
            source->name().to_string(), output->name, dest->name().to_string(),
            input->name);
  EventData e;
  e.source = this;
  e.type = EventType::ConnectionAdded;
  e.u.connectionAdded.source = source;
  e.u.connectionAdded.output = output;
  e.u.connectionAdded.dest = dest;
  e.u.connectionAdded.input = input;
  notify(e);
}

void Node::onConnectionRemoved(Node *source, Output *output, Node *dest,
                               Input *input) {
  util::log("Node[{}]::onConnectionRemoved({},{},{},{})", name().to_string(),
            source->name().to_string(), output->name, dest->name().to_string(),
            input->name);
  EventData e;
  e.source = this;
  e.type = EventType::ConnectionRemoved;
  e.u.connectionAdded.source = source;
  e.u.connectionAdded.output = output;
  e.u.connectionAdded.dest = dest;
  e.u.connectionAdded.input = input;
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

/*void Node::referenceDeleted(Node *ref, Output *output) {
  for (auto &&input : inputs_) {
    if (input->nodeRef_.target_ == ref && input->nodeRef_.output_ == output) {
      disconnectInput(input.get());
    }
  }
}*/

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

Input *Node::input(int index) { return inputs_[index].get(); }

Input *Node::input(util::StringRef name) {
  for (auto &&i : inputs_) {
    if (i->name == name) {
      return i.get();
    }
  }
  return nullptr;
}

Output *Node::output(int index) { return outputs_[index].get(); }

Output *Node::output(util::StringRef name) {
  for (auto &&o : outputs_) {
    if (o->name == name) {
      return o.get();
    }
  }
  return nullptr;
}

util::StringRef Node::inputName(Input *input) { return input->name; }
util::StringRef Node::outputName(Output *output) { return output->name; }

int Node::inputUniqueId(Input *input) { return input->id; }
int Node::outputUniqueId(Output *output) { return output->id; }

bool Node::isInputConnected(Input *input) { return input->output != nullptr; }

bool Node::inputSource(Input *input, Node *&node, Output *&output) {
  if (!isInputConnected(input))
    return false;
  node = input->source;
  output = input->output;
  return true;
}

// ===================================================================
// Parameters

Param *Node::createParameter(const ParamDesc& desc) {
	auto param = new Param{ this, desc };
	params_.push_back(std::unique_ptr<Param>(param));
	return param;
}

void Node::deleteParameter(Param *p) { eraseRemoveUniquePtr(params_, p); }

const Value& Node::evalParam(Param &p) {
	// TODO
	return p.value();
}

const Value& Node::evalParam(util::StringRef name) {
	for (auto&& p : params_) {
		if (p->desc().name() == name) {
			return evalParam(*p);
		}
	}
	// oops
	return Value::EMPTY;
}

const Value& Node::evalParam(const ParamDesc& param) {
	for (auto&& p : params_) {
		if (&p->desc() == &param) {
			return evalParam(*p);
		}
	}
	// oops
	return Value::EMPTY;
}


// ===================================================================

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

// ===================================================================
// Serialization

void Node::load(util::JsonReader &r, int baseId) {
  r.beginObject();
  while (r.hasNext()) {
    auto k = r.nextName();
    if (k == "name") {
      setName(r.nextString());
    } else if (k == "id") {
      int id = (int)r.nextInt();
      id_ = baseId + id;
    } else if (k == "inputs") {
      r.beginArray();
      while (r.hasNext()) {
        std::string name;
        int id;
        r.beginObject();
        while (r.hasNext()) {
          auto k = r.nextName();
          if (k == "name") {
            name = r.nextString();
          } else if (k == "id") {
            id = (int)r.nextInt();
          } else {
            util::log("WARNING Node::load: unknown attribute {}", k);
            r.skipValue();
          }
        }
        r.endObject();
        createInputInternal(name, id);
      }
      r.endArray();
    } else if (k == "outputs") {
      r.beginArray();
      while (r.hasNext()) {
        std::string name;
        int id;
        r.beginObject();
        while (r.hasNext()) {
          auto k = r.nextName();
          if (k == "name") {
            name = r.nextString();
          } else if (k == "id") {
            id = (int)r.nextInt();
          } else {
            util::log("WARNING Node::load: unknown attribute {}", k);
            r.skipValue();
          }
        }
        r.endObject();
        createOutputInternal(name, id);
      }
      r.endArray();
    } else {
      // loadInternal(nextName, r)
      util::log("WARNING Node::load: unknown attribute {}", k);
      r.skipValue();
    }
  }
  r.endObject();
}

void Node::save(util::JsonWriter &w) {
  w.beginObject();
  w.name("name");
  w.value(name());
  w.name("id");
  w.value((int64_t)id_);
  w.name("inputs");
  w.beginArray();
  for (auto &&in : inputs_) {
    w.beginObject();
    w.name("name");
    w.value(in->name);
    w.name("id");
    w.value((int64_t)in->id);
    w.endObject();
  }
  w.endArray();
  w.name("outputs");
  w.beginArray();
  for (auto &&out : outputs_) {
    w.beginObject();
    w.name("name");
    w.value(out->name);
    w.name("id");
    w.value((int64_t)out->id);
    w.endObject();
  }
  w.endArray();
  saveInternal(w);
  w.endObject();
}

void Node::loadInternal(util::StringRef key, util::JsonReader &reader) {}
void Node::saveInternal(util::JsonWriter &writer) {}

} // namespace node