#pragma once
#include "node/param.h"
#include "util/stringref.h"
#include <string>
#include <vector>

namespace node {
class Node;
class Network;

using Constructor = Node *(*)(Network &parent, util::StringRef name);

//------------------------------------------------------------------------------------

class NodeDescription {
  friend class NodeDescriptions;

public:
  NodeDescription(util::StringRef name, util::StringRef friendlyName,
                  util::StringRef description, Constructor constructor)
      : name_{name.to_string()}, friendlyName_{friendlyName.to_string()},
        description_{description.to_string()}, constructor_{constructor} {}

  Node *instantiate(Network &net, util::StringRef name) {
    return constructor_(net, name);
  }

  util::StringRef typeName() const { return name_; }
  util::StringRef friendlyName() const { return friendlyName_; }
  util::StringRef description() const { return description_; }

private:
  std::string name_;
  std::string friendlyName_;
  std::string description_;
  Constructor constructor_;
};

/// Each network has a table that define constructors for the
/// nodes inside the network.
class NodeDescriptions {
public:
  void             registerNode(std::unique_ptr<NodeDescription> tpl);
  void             registerNode(util::StringRef name, util::StringRef friendlyName,
								util::StringRef description, Constructor constructor);
  NodeDescription *find(util::StringRef type);

  int              count();
  NodeDescription &at(int index);

private:
  std::vector<std::unique_ptr<NodeDescription>> descriptions_;
};

} // namespace node