#pragma once
#include "util/stringref.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace node {

class Node;
class Param;
class Observer;
class Network;
class Output;
class Input;

class Input {
	friend class Node;

	using Ptr = std::unique_ptr<Input>;

	int id = 0;
	std::string name = "";

	bool showConnector = false;

	std::string sourcePath = "";
	std::string sourceOutputName = "";

	Node *source = nullptr;
	Output *output = nullptr;
};

class Output {
	friend class Node;

	using Ptr = std::unique_ptr<Output>;

	int id = 0;
	std::string name;
	std::vector<Node *> dependents;
};


enum class EventType {
  /// A child node has been added.
  ChildAdded,
  /// A child node is about to be removed.
  ChildRemoved,
  /// A reference to a node has been added.
  ReferenceAdded,
  /// A reference to a node is about to be removed.
  ReferenceRemoved,
  /// This node is being deleted.
  NodeDeleted,
  InputAdded,
  InputRemoved,
  OutputAdded,
  OutputRemoved,
  /// A connection has been added in this network
  ConnectionAdded,
  /// A connection is about to be removed in this network
  ConnectionRemoved,
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
	struct {
		Node* source;
		Output* output;
		Node* dest;
		Input* input;
	} connectionAdded;
	struct {
		Node* source;
		Output* output;
		Node* dest;
		Input* input;
	} connectionRemoved;
  } u;
};

using EventHandler = void(const EventData &);

using ParamMap = std::map<std::string, Param *>;
using InputMap = std::map<std::string, Input *>;
using OutputMap = std::map<std::string, Output *>;


/*
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
};*/

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
  friend class Observer;
  friend class Param;
  friend class Network;

public:
  using Ptr = std::unique_ptr<Node>;

  Node(Network *parent);
  Node(std::string name, Network *parent);
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

  Input *createInput(std::string name);
  void deleteInput(Input *input);
  void connectInput(Input *input, Node* source, Output* output);
  void connectInput(Input *input, std::string node, std::string output);
  bool resolveInput(Input* input);
  void disconnectInput(Input* input);
  int referenceCount(Node* source, Output* output);
  Output *createOutput(std::string name);
  void deleteOutput(Output *output);
  void disconnectOutput(Output* output);
  Param *createParameter(std::string name, std::string description,
                         double initValue = 0.0);
  void deleteParameter(Param *p);

  // Inputs and outputs
  Input *input(int id);
  Input *input(util::StringRef name);
  Output *output(int id);
  Output *output(util::StringRef name);

protected:
  void onChildAdded(Node *node);
  void onChildRemoved(Node *node);
  void onReferenceAdded(Node *to);
  void onReferenceRemoved(Node *to);
  void onNodeDeleted();
  void onInputAdded(Input *input);
  void onInputRemoved(Input *input);
  void onOutputAdded(Output *output);
  void onOutputRemoved(Output *output);
  void onConnectOutput(Output *output, Node *targetNode);
  void onDisconnectOutput(Output *output, Node *targetNode);
  void onConnectionAdded(Node* source, Output* output, Node *dest, Input* input);
  void onConnectionRemoved(Node* source, Output* output, Node *dest, Input* input);
  /// Notifies all observers about an event.
  void notify(const EventData &e);

private:
	void addDependentNode(Output* output, Node* destination, Input* input);
	void removeDependentNode(Output* output, Node* destination, Input* input);
	void doDisconnectInput(Input* input, bool removeReferenceFromOutput);
	std::string makeUniqueInputName(std::string s, int id);
	std::string makeUniqueOutputName(std::string s, int id);
	
  void addObserver(Observer *obs);
  void removeObserver(Observer *obs);

  int getInputId() { return inputIdCounter_++; }
  int getOutputId() { return outputIdCounter_++; }

  bool dirty_ = true;
  std::string name_;
  int outputIdCounter_ = 0;
  int inputIdCounter_ = 0;
  // unique ID across all networks, used for serialization.
  int id_ = 0;

  // parent of this node, or nullptr if this is the root network
  Network *parent_ = nullptr;
  std::vector<Param::Ptr> params_;
  std::vector<std::unique_ptr<Input>> inputs_;
  std::vector<std::unique_ptr<Output>> outputs_;
  
  // observers
  int notifying_ = 0;
  std::vector<Observer *> observers_;
  std::vector<Observer *> observersToAdd_;
  std::vector<Observer *> observersToRemove_;
};

} // namespace render
