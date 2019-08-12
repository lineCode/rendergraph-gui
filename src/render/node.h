#pragma once
#include "gfx/gfx.h"
#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace render {

class Node;
class Param;
class NodeRef;
class Observer;

enum class EventType {
  ChildAdded,
  ChildRemoved,
  ReferenceAdded,
  ReferenceRemoved,
  NodeDeleted,
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
  } u;
};

using EventHandler = void(const EventData &);

/// Base class for all nodes.
///
/// The main purpose of this class is to unify the way dependencies between
/// operations are handled.
class Node {
  friend class NodeRef;
  friend class Observer;

public:
  using Ptr = std::unique_ptr<Node>;

  Node() {}
  Node(std::string name) : name_{name} {}
  virtual ~Node();

  // Name
  util::StringRef name() const;
  void setName(std::string name);

  /// Marks this node as dirty.
  void markDirty();

  /// Returns the parent of this node.
  Node *parent() const;

  /// Adds a child node
  Node *addChild(Ptr ptr);

  /// Deletes child nodes.
  void deleteNode(Node *node);
  void deleteNodes(util::ArrayRef<Node * const> nodes);

  /// Returns a vector containing all child nodes of the specified type
  template <typename T,
            typename = std::enable_if_t<std::is_base_of<Node, T>::value>>
  std::vector<T *> findChildrenByType() {
    std::vector<T *> results;
    for (auto &&child : children_) {
      auto pchild = child.get();
      if (auto derived = dynamic_cast<T *>(pchild)) {
        results.push_back(derived);
      }
    }
    return results;
  }

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

  void notify(const EventData &e);

private:
  void addReference(NodeRef *ref) { references_.push_back(ref); }

  void removeReference(NodeRef *ref) {
    auto it = std::remove(references_.begin(), references_.end(), ref);
    references_.erase(it, references_.end());
  }

  void addObserver(Observer *obs) {
    if (notifying_) {
      observersToAdd_.push_back(obs);
    } else {
      observers_.push_back(obs);
    }
  }

  void removeObserver(Observer *obs) {
    if (notifying_) {
      observersToRemove_.push_back(obs);
    } else {
      auto it = std::remove(observers_.begin(), observers_.end(), obs);
      observers_.erase(it, observers_.end());
    }
  }

  bool dirty_ = true;
  std::string name_;
  Node *parent_ =
      nullptr; // parent of this node, or nullptr if this is the root node
  std::vector<Ptr> children_;             // nodes contained inside this node
  std::map<std::string, Param *> params_; // node parameters

  std::vector<NodeRef *> references_; // nodes referenced by this node (which
                                      // nodes this node depends on?)
  std::vector<Node *>
      dependents_; // nodes that depend on this node (they are notified when
                   // this node is being changed or deleted)

  int notifying_ = 0;
  std::vector<Observer *> observers_;
  std::vector<Observer *> observersToAdd_;
  std::vector<Observer *> observersToRemove_;
};

class Observer {
  friend class Node;

public:
  using Ptr = std::unique_ptr<Observer>;

  Observer(Node *observed, std::function<EventHandler> callback)
      : observed_{observed}, callback_{std::move(callback)} {
    observed_->addObserver(this);
  }

  ~Observer() { observed_->addObserver(this); }

  static Ptr make(Node *observed, std::function<EventHandler> callback) {
    return std::make_unique<Observer>(observed, std::move(callback));
  }

private:
  Node *observed_;
  std::function<EventHandler> callback_;
};

/// A reference to a node (by name).
class NodeRef {
public:
  using Ptr = std::unique_ptr<NodeRef>;

  NodeRef(Node *owner, std::string path)
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

/// Parameters are nodes that are displayed in the
/// parameter panel.
class Param : public Node {
public:
  Param(std::string name, std::string description, double initValue)
      : Node{std::move(name)}, val_{initValue}, description_{
                                                    std::move(description_)} {}

  double value() const { return val_; }

  void setValue(double newValue) { val_ = newValue; }

  static Param *make(Node *parent, std::string name, std::string description,
                     double initValue) {
    auto p = std::make_unique<Param>(std::move(name), std::move(description),
                                     std::move(initValue));
    return static_cast<Param *>(parent->addChild(std::move(p)));
  }

  util::StringRef description() const { return description_; }

private:
  // TODO support more things
  double val_;
  std::string description_;
};

} // namespace render
