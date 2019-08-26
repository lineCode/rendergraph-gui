#pragma once
#include "util/jsonreader.h"
#include "util/jsonwriter.h"
#include "util/stringref.h"
#include "util/value.h"
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
class Blueprint;
class ParamDesc;

class Input {
  friend class Node;
  using Ptr = std::unique_ptr<Input>;

  int         id = 0;
  std::string name = "";
  bool        showConnector = false;
  std::string sourcePath = "";
  std::string sourceOutputName = "";
  Node *      source = nullptr;
  Output *    output = nullptr;
};

class Output {
  friend class Node;

  using Ptr = std::unique_ptr<Output>;

  int                 id = 0;
  std::string         name;
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
  Node *    source;
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
      Node *  source;
      Output *output;
      Node *  dest;
      Input * input;
    } connectionAdded;
    struct {
      Node *  source;
      Output *output;
      Node *  dest;
      Input * input;
    } connectionRemoved;
  } u;
};

using EventHandler = void(const EventData &);

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

  // Node(Network *parent);
  Node(node::Network *parent, std::string name, node::Blueprint *blueprint);
  virtual ~Node();

  // Name
  util::StringRef name() const;
  void            setName(std::string name);
  int             uniqueId();

  /// Marks this node as dirty.
  void markDirty();

  /// Returns the parent of this node.
  Network *parent() const;

  // Inputs/outputs
  Input * createInput(std::string name);
  void    deleteInput(Input *input);
  void    connectInput(Input *input, Node *source, Output *output);
  void    connectInput(Input *input, std::string node, std::string output);
  bool    resolveInput(Input *input);
  void    disconnectInput(Input *input);
  int     referenceCount(Node *source, Output *output);
  Output *createOutput(std::string name);
  void    deleteOutput(Output *output);
  void    disconnectOutput(Output *output);

  int             inputCount() const { return (int)inputs_.size(); }
  Input *         input(int index);
  Input *         input(util::StringRef name);
  int             outputCount() const { return (int)outputs_.size(); }
  Output *        output(int index);
  Output *        output(util::StringRef name);
  util::StringRef inputName(Input *input);
  util::StringRef outputName(Output *output);
  int             inputUniqueId(Input *input);
  int             outputUniqueId(Output *output);
  bool            isInputConnected(Input *input);
  bool            inputSource(Input *input, Node *&node, Output *&output);

  // Parameters
  Param *createParameter(const ParamDesc &desc);
  void   deleteParameter(Param *p);
  int    paramCount() const;
  Param *param(int index);
  Param *param(const ParamDesc &param);
  Param *param(util::StringRef name);

  const util::Value &evalParam(Param &p);
  const util::Value &evalParam(util::StringRef name);
  const util::Value &evalParam(const ParamDesc &param);
  void setParam(util::StringRef name, util::Value value);
  void setParam(const ParamDesc& param, util::Value value);
  void setParam(Param& p, util::Value value);

  // load/save
  void load(util::JsonReader &reader, int baseId);
  void save(util::JsonWriter &writer);

protected:
  Input * createInputInternal(std::string name, int uid);
  Output *createOutputInternal(std::string name, int uid);

  virtual void loadInternal(util::StringRef key, util::JsonReader &reader);
  virtual void saveInternal(util::JsonWriter &writer);

  void         notify(const EventData &e);
  virtual void onChildAdded(Node *node);
  virtual void onChildRemoved(Node *node);
  virtual void onReferenceAdded(Node *to);
  virtual void onReferenceRemoved(Node *to);
  virtual void onNodeDeleted();
  virtual void onInputAdded(Input *input);
  virtual void onInputRemoved(Input *input);
  virtual void onOutputAdded(Output *output);
  virtual void onOutputRemoved(Output *output);
  virtual void onConnectOutput(Output *output, Node *targetNode);
  virtual void onDisconnectOutput(Output *output, Node *targetNode);
  virtual void onConnectionAdded(Node *source, Output *output, Node *dest,
                                 Input *input);
  virtual void onConnectionRemoved(Node *source, Output *output, Node *dest,
                                   Input *input);

private:
  void addDependentNode(Output *output, Node *destination, Input *input);
  void removeDependentNode(Output *output, Node *destination, Input *input);
  void doDisconnectInput(Input *input, bool removeReferenceFromOutput);
  std::string makeUniqueInputName(std::string s, int id);
  std::string makeUniqueOutputName(std::string s, int id);

  void addObserver(Observer *obs);
  void removeObserver(Observer *obs);

  int getInputId() { return inputIdCounter_++; }
  int getOutputId() { return outputIdCounter_++; }

  bool        dirty_ = true;
  std::string name_;
  Blueprint * blueprint_;
  int         outputIdCounter_ = 0;
  int         inputIdCounter_ = 0;
  // unique ID across all networks, used for serialization.
  int id_ = 0;

  // parent of this node, or nullptr if this is the root network
  Network *                            parent_ = nullptr;
  std::vector<std::unique_ptr<Param>>  params_;
  std::vector<std::unique_ptr<Input>>  inputs_;
  std::vector<std::unique_ptr<Output>> outputs_;

  // observers
  int                     notifying_ = 0;
  std::vector<Observer *> observers_;
  std::vector<Observer *> observersToAdd_;
  std::vector<Observer *> observersToRemove_;
};

} // namespace node
