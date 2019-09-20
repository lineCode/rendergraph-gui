#include "description.h"
#include "util/log.h"

namespace node {
//----------------------------------------------------------------------------------
void NodeDescriptions::registerNode(std::unique_ptr<NodeDescription> desc) {
  // check for duplicate name
  if (find(desc->name_)) {
    util::log("WARNING NodeDescriptions::registerDescription: duplicate "
              "descriptions with "
              "name `{}`",
              desc->name_);
    return;
  }
  descriptions_.push_back(std::move(desc));
}

void NodeDescriptions::registerNode(util::StringRef name,
                                    util::StringRef friendlyName,
                                    util::StringRef description,
                                    Constructor     constructor) {
  registerNode(std::make_unique<NodeDescription>(name, friendlyName,
                                                 description, constructor));
}

NodeDescription *NodeDescriptions::find(util::StringRef name) {
  for (auto &&tpl : descriptions_) {
    if (tpl->name_ == name) {
      return tpl.get();
    }
  }
  return nullptr;
}

int              NodeDescriptions::count() { return (int)descriptions_.size(); }
NodeDescription &NodeDescriptions::at(int index) {
  return *descriptions_[index];
}

} // namespace node