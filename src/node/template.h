#pragma once
#include "node/param.h"
#include "util/stringref.h"
#include <string>
#include <vector>

namespace node {
class Node;
class Network;

using Constructor = Node *(*)(Network &parent, util::StringRef name,
                              NodeTemplate &tpl);

struct InputDesc {
  std::string name;
};

struct OutputDesc {
  std::string name;
};

//------------------------------------------------------------------------------------

/// A node template contains all information needed by the `Node` constructor
/// to create inputs, outputs, and parameters.
///
/// There are "built-in" node templates for nodes implemented in C++.
/// Eventually, the user will be able to create their own "Node types",
/// represented by node templates.
class NodeTemplate {
  friend class TemplateTable;

public:
  NodeTemplate(util::StringRef name, util::StringRef friendlyName,
               util::StringRef description, util::StringRef icon,
               util::ArrayRef<const ParamDesc *>  params,
               util::ArrayRef<const InputDesc *>  inputs,
               util::ArrayRef<const OutputDesc *> outputs,
               Constructor                        constructor)
      : name_{name.to_string()}, friendlyName_{friendlyName.to_string()},
        description_{description.to_string()}, icon_{icon.to_string()},
        params_{params}, inputs_{inputs}, outputs_{outputs}, constructor_{
                                                                 constructor} {}

  Node *make(Network &net, util::StringRef name) {
    return constructor_(net, name, *this);
  }

  util::StringRef typeName() const { return name_; }
  util::StringRef friendlyName() const { return friendlyName_; }
  util::StringRef description() const { return description_; }
  util::StringRef icon() const { return icon_; }
  util::ArrayRef<const ParamDesc *> params() const { return params_; }
  util::ArrayRef<const InputDesc *> inputs() const { return inputs_; }
  util::ArrayRef<const OutputDesc *> outputs() const { return outputs_; }

private:
  std::string                        name_;
  std::string                        friendlyName_;
  std::string                        description_;
  std::string                        icon_;
  util::ArrayRef<const ParamDesc *>  params_;
  util::ArrayRef<const InputDesc *>  inputs_;
  util::ArrayRef<const OutputDesc *> outputs_;
  Constructor                        constructor_;
};

/// Each network has a table that define constructors for the
/// nodes inside the network.
class TemplateTable {
public:
  void          registerTemplate(std::unique_ptr<NodeTemplate> tpl);
  NodeTemplate *findTemplate(util::StringRef type);

  int           count();
  NodeTemplate &at(int index);

private:
  std::vector<std::unique_ptr<NodeTemplate>> templates_;
};

} // namespace node