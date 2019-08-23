#pragma once
#include "node/param.h"
#include "util/stringref.h"
#include <string>
#include <vector>

namespace node {
class Node;
class Network;

//------------------------------------------------------------------------------------
class Blueprint {
  friend class BlueprintTable;

public:
  Blueprint(std::string name, std::string friendlyName, std::string description,
            std::string icon, util::ArrayRef<const ParamDesc> params,
            Node *(*constructor)(Network &, std::string, Blueprint &))
      : type_{std::move(name)}, friendlyName_{std::move(friendlyName)},
        description_{std::move(description)}, icon_{std::move(icon)},
        params_{params}, constructor_{constructor} {}

  Node *make(Network &net, std::string name) {
    return constructor_(net, std::move(name), *this);
  }

  util::StringRef typeName() const { return type_; }
  util::StringRef friendlyName() const { return friendlyName_; }
  util::StringRef description() const { return description_; }
  util::StringRef icon() const { return icon_; }

private:
  std::string type_;
  std::string friendlyName_;
  std::string description_;
  std::string icon_;
  util::ArrayRef<const ParamDesc> params_;
  Node *(*constructor_)(Network &, std::string, Blueprint &);
};

/// Each network has a table of blueprints that define constructors for the
/// nodes inside the network.
class BlueprintTable {
public:
  void registerBlueprint(Blueprint *blueprint);
  Blueprint *findBlueprint(util::StringRef type);

  int count();
  Blueprint &at(int index);

private:
  // TODO does the blueprint table has to own the blueprints?
  // can't it just store references to static variables?
  std::vector<std::unique_ptr<Blueprint>> blueprints_;
};

} // namespace node