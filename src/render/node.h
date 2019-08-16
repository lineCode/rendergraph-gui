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
  /// A
  ChildAdded,
  ChildRemoved,
  ReferenceAdded,
  ReferenceRemoved,
  NodeDeleted,
  InputAdded,
  InputRemoved,
  OutputAdded,
  OutputRemoved
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
      Input *input;
    } inputAdded;
    struct {
      Input *input;
    } inputRemoved;
    struct {
      Output *output;
    } outputAdded;
    struct {
      Output *output;
    } outputRemoved;
  } u;
};

using EventHandler = void(const EventData &);

using ParamMap = std::map<std::string, Param *>;
using InputMap = std::map<std::string, Input *>;
using OutputMap = std::map<std::string, Output *>;

/// A weak reference to an output of a node (by name).
class NodeRef {
  friend class Node;

public:
  using Ptr = std::unique_ptr<NodeRef>;
  NodeRef(Node *owner, std::string path = "", std::string outputName = "");
  ~NodeRef();

  bool set(std::string path, std::string outputName) {
    disconnect();
    path_ = std::move(path);
    outputName_ = std::move(outputName);
    return resolve();
  }

private:
  bool resolve();

  void disconnect();
  /// Called when the referenced output is about to be deleted.
  void referenceDeleted();

  Node *owner_;
  Node *target_;
  Output *output_;
  std::string path_;
  std::string outputName_;
};

/// The input of a node that shows up as a connector in the node graph.
class Input {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Input>;

  Input(Node *owner, int id, std::string name)
      : nodeRef_{owner}, id_{id}, name_{name} {}
  ~Input() {}

  int id() const { return id_; }
  util::StringRef name() const { return name_; }

  void set(std::string node, std::string output) { nodeRef_.set(node, output); }

  static Ptr make(Node *owner, int id, std::string name) {
    return std::make_unique<Input>(owner, id, std::move(name));
  }

private:
  int id_;
  std::string name_;
  NodeRef nodeRef_;
};

/// The output of a node that shows up as a connector in the node graph.
class Output {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Output>;

  Output(int id, std::string name) : id_{id}, name_{name} {}
  ~Output() {}

  int id() const { return id_; }
  util::StringRef name() const { return name_; }

  void addDependent(Node *node) { dependents_.push_back(node); }

  void removeDependent(Node *node) {
    auto it = std::remove(dependents_.begin(), dependents_.end(), node);
    dependents_.erase(it, dependents_.end());
  }

  static Ptr make(int id, std::string name) {
    return std::make_unique<Output>(id, std::move(name));
  }

private:
  int id_;
  std::string name_;
  std::vector<Node *> dependents_; // dependent nodes
};

/// Parameters are displayed in the parameter panel.
class Param {
public:
  using Ptr = std::unique_ptr<Param>;
  Param(Node *owner, std::string name, std::string description,
        double initValue)
      : owner_{owner}, name_{name}, val_{initValue}, description_{std::move(
                                                         description_)} {}

  ~Param() {}

  util::StringRef name() const { return name_; }
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

// helpers
template <typename Container, typename T>
void eraseRemoveUniquePtr(Container &container, const T *val) {
  static_assert(
      std::is_same<typename Container::value_type, std::unique_ptr<T>>::value,
      "Container value type must be unique_ptr<T>");
  auto it = std::remove_if(container.begin(), container.end(),
                           [val](const std::unique_ptr<T> &ptr) {
                             if (ptr.get() == val) {
                               return true;
                             }
                             return false;
                           });
  container.erase(it, container.end());
}

template <typename Container, typename T>
void eraseRemoveId(Container &container, int id) {
  static_assert(
      std::is_same<typename Container::value_type, std::unique_ptr<T>>::value,
      "Container value type must be unique_ptr<T>");
  static_assert() auto it = std::remove_if(
      container.begin(), container.end(), [val](const std::unique_ptr<T> &ptr) {
        if (ptr->id() == id) {
          return true;
        }
        return false;
      });
  container.erase(it, container.end());
}

template <typename Container, typename T>
T *pushUniquePtr(Container &container, std::unique_ptr<T> ptr) {
  static_assert(
      std::is_same<typename Container::value_type, std::unique_ptr<T>>::value,
      "Container value type must be unique_ptr<T>");
  auto p = ptr.get();
  container.push_back(std::move(ptr));
  return p;
}

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
  void disconnect();

  Input *createInput(std::string name);
  void deleteInput(Input *input);
  Output *createOutput(std::string name);
  void deleteOutput(Output *output);
  Param *createParameter(std::string name, std::string description,
                         double initValue = 0.0);
  void deleteParameter(Param *p);

  // Inputs and outputs
  Input *input(int id);
  Input *input(util::StringRef name);
  Output *output(int id);
  Output *output(util::StringRef name);
  void connectInput(Input *input, Node *source, Output *sourceOutput);

protected:
  /// A child node has been added.
  void onChildAdded(Node *node);
  /// A child node is about to be removed.
  void onChildRemoved(Node *node);
  /// A reference to a node has been added.
  void onReferenceAdded(Node *to);
  /// A reference to a node is about to be removed.
  void onReferenceRemoved(Node *to);
  /// This node is being deleted.
  void onNodeDeleted();
  ///
  void onInputAdded(Input *input);
  void onInputRemoved(Input *input);
  void onOutputAdded(Output *output);
  void onOutputRemoved(Output *output);
  /// A dependent node has been added on the specified output.
  void onConnectOutput(Node *node, Output *output);
  /// A dependent node is about to be removed from the specified output.
  void onDisconnectOutput(Node *node, Output *output);

  /// Notifies all observers about an event.
  void notify(const EventData &e);

  /// Called when a referenced node has been deleted, or a referenced output
  /// has been deleted.
  void referenceDeleted(Node *ref, Output *output);

private:
  /// Plugs a dependent node into the specified output. The output
  /// should belong to this node.
  void connectOutput(Node *dep, Output *output);
  /// Unplugs a dependent node from the specified output. The output
  /// should belong to this node.
  void disconnectOutput(Node *dep, Output *output);

  void addReference(NodeRef *ref);
  void removeReference(NodeRef *ref);
  void addObserver(Observer *obs);
  void removeObserver(Observer *obs);

  int getInputId() { return inputIdCount_++; }
  int getOutputId() { return outputIdCount_++; }

  bool dirty_ = true;
  std::string name_;
  int outputIdCount_ = 0;
  int inputIdCount_ = 0;
  // unique ID across all networks, used for serialization.
  int id_ = 0;

  // parent of this node, or nullptr if this is the root network
  Network *parent_ = nullptr;
  std::vector<Param::Ptr> params_;
  std::vector<Input::Ptr> inputs_;
  std::vector<Output::Ptr> outputs_;
  std::vector<NodeRef *> references_;

  // observers
  int notifying_ = 0;
  std::vector<Observer *> observers_;
  std::vector<Observer *> observersToAdd_;
  std::vector<Observer *> observersToRemove_;
};

} // namespace render
