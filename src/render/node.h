#pragma once
#include "util/stringref.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace render {

class Node;
class Param;
class NodeRef;
class Observer;
class Network;
class Input;
class Output;

enum class EventType {
  ChildAdded,
  ChildRemoved,
  ReferenceAdded,
  ReferenceRemoved,
  NodeDeleted,
  InputAdded,
  InputRemoved
};

struct EventData {
  Node *source;
  EventType type;
  union {
    struct {
      Node *node;
    } childAdded;
    struct {
      Node *node;
    } childRemoved;
    struct {
      Node *to;
    } referenceAdded;
    struct {
      Node *to;
    } referenceRemoved;
    struct {
      int index;
    } inputAdded;
    struct {
      int index;
    } inputRemoved;
  } u;
};

using EventHandler = void(const EventData &);

using ParamMap = std::map<std::string, Param *>;
using InputMap = std::map<std::string, Input *>;
using OutputMap = std::map<std::string, Output *>;

/// Base class for all nodes.
///
/// The main purpose of this class is to unify the way dependencies between
/// operations are handled.
class Node {
  friend class NodeRef;
  friend class Observer;
  friend class Input;
  friend class Output;
  friend class Param;
  friend class Network;

public:
  using Ptr = std::unique_ptr<Node>;

  Node(Network *parent) : parent_{parent} {}

  Node(std::string name, Network *parent) : name_{name}, parent_{parent} {}
  virtual ~Node();

  // Name
  util::StringRef name() const;
  void setName(std::string name);

  /// Marks this node as dirty.
  void markDirty();

  /// Returns the parent of this node.
  Network *parent() const;

  /// Parameters
  int paramCount() const;
  Param *param(int index);

  // sever all connections
  void disconnect() const;

protected:
  // child added
  void onChildAdded(Node *node);
  // child about to be removed
  void onChildRemoved(Node *node);
  void onReferenceAdded(Node *to);
  void onReferenceRemoved(Node *to);
  void onNodeDeleted();
  void onInputAdded(int index);
  void onInputRemoved(int index);
  void onOutputAdded(int index);
  void onOutputRemoved(int index);

  void notify(const EventData &e);

private:
  void addParameter(Param *p);
  void removeParameter(Param *p);
  void addReference(NodeRef *ref);
  void removeReference(NodeRef *ref);
  void addObserver(Observer *obs);
  void removeObserver(Observer *obs);
  void insertInput(Input *input, int index = -1);
  void removeInput(Input *input);
  void insertOutput(Output *output, int index = -1);
  void removeOutput(Output *output);

  bool dirty_ = true;
  std::string name_;

  // parent of this node, or nullptr if this is the root network
  Network *parent_ = nullptr;

  std::vector<Param *> params_;
  std::vector<Input *> inputs_;
  std::vector<Output *> outputs_;

  // nodes referenced by this node (which nodes this node depends on?)
  std::vector<NodeRef *> references_;
  // nodes that depend on this node (they are notified when
  // this node is being changed or deleted)
  std::vector<Node *> dependents_;

  // observers
  int notifying_ = 0;
  std::vector<Observer *> observers_;
  std::vector<Observer *> observersToAdd_;
  std::vector<Observer *> observersToRemove_;
};

/// A reference to a node (by name).
class NodeRef {
public:
  using Ptr = std::unique_ptr<NodeRef>;

  NodeRef(Node *owner, std::string path = "")
      : owner_{owner}, path_{std::move(path)} {
    owner_->addReference(this);
  }

  ~NodeRef() { owner_->removeReference(this); }

  ///
  Node *resolve() {
    // TODO
    return nullptr;
  }

private:
  // node that owns the reference
  Node *owner_;
  std::string path_;
};

/// Parameters are displayed in the parameter panel.
class Param {
public:
  using Ptr = std::unique_ptr<Param>;
  Param(Node *owner, std::string name, std::string description,
        double initValue)
      : owner_{owner}, name_{name}, val_{initValue}, description_{std::move(
                                                         description_)} {
    owner_->addParameter(this);
  }

  ~Param() { owner_->removeParameter(this); }

  util::StringRef name() const {
	  return name_;
  }
  double value() const { return val_; }
  void setValue(double newValue) { val_ = newValue; }
  util::StringRef description() const { return description_; }

  static Ptr make(Node *owner, std::string name, std::string description,
                  double initValue) {
    return std::make_unique<Param>(owner, std::move(name),
                                   std::move(description), initValue);
  }

private:
  Node *owner_;
  std::string name_;
  double val_;
  std::string description_;
};

} // namespace render
