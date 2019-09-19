#include "node/network.h"
#include "fmt/format.h"
#include "util/log.h"
#include <algorithm>

namespace node {

Node *Network::addChild(Node *ptr) {
  makeNameUnique(ptr->name_);
  children_.push_back(std::unique_ptr<Node>(ptr));
  onChildAdded(ptr);
  return ptr;
}

void Network::deleteChild(Node *node) {
  Node *nodes[1] = {node};
  deleteChildren(util::makeConstArrayRef(nodes));
}

void Network::deleteChildren(util::ArrayRef<Node *const> nodes) {
  auto it = std::remove_if(children_.begin(), children_.end(),
                           [this, nodes](const std::unique_ptr<Node> &ptr) {
                             for (auto n : nodes) {
                               if (ptr.get() == n) {
                                 return true;
                               }
                             }
                             return false;
                           });
  // for (auto it2 = it; it2 < children_.end(); ++it2) {
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

void Network::addConnection(util::StringRef from, util::StringRef fromOutput,
                            util::StringRef to, util::StringRef toInput) {
  auto toNode = findChildByName(to);
  if (!toNode)
    return;
  auto input = toNode->input(toInput);
  if (!input)
    return;
  toNode->connectInput(input, from.to_string(), fromOutput.to_string());
}

void Network::addConnection(Node *source, Output *output, Node *destination,
                            Input *input) {
  destination->connectInput(input, source, output);
}

void Network::makeNameUnique(std::string &name) {
  for (auto &&c : children_) {
    if (c->name_ == name) {
      name.append(fmt::format("_{}", uniqueNameCounter_++));
      break;
    }
  }
}

void Network::loadInternal(util::StringRef key, util::JsonReader &r) {}

void Network::saveInternal(util::JsonWriter &w) {
  w.name("children");
  w.beginArray();
  for (auto &&c : children_) {
    c->save(w);
  }
  w.endArray();
  w.name("connections");
  w.beginArray();
  for (auto &&c : children_) {
    int n = c->inputCount();
    for (int i = 0; i < n; ++i) {
      Input *in = c->input(i);
      Node *source;
      Output *out;
      if (inputSource(in, source, out)) {
        w.value((int64_t)source->uniqueId());
        w.value((int64_t)source->outputUniqueId(out));
        w.value((int64_t)c->uniqueId());
        w.value((int64_t)c->inputUniqueId(in));
      }
    }
  }
  w.endArray();
}

} // namespace node