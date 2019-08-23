#include "blueprint.h"
#include "util/log.h"

namespace node {
//----------------------------------------------------------------------------------
void BlueprintTable::registerBlueprint(Blueprint *blueprint) {
  // check for duplicate name
  if (findBlueprint(blueprint->type_)) {
    util::log("WARNING Node::registerBlueprint: duplicate blueprints with "
              "type `{}`",
              blueprint->type_);
    return;
  }
  blueprints_.push_back(std::unique_ptr<Blueprint>(blueprint));
}

Blueprint *BlueprintTable::findBlueprint(util::StringRef type) {
  for (auto &&bp : blueprints_) {
    if (bp->type_ == type) {
      return bp.get();
    }
  }
  return nullptr;
}

int BlueprintTable::count() { return blueprints_.size(); }

Blueprint &BlueprintTable::at(int index) { return *blueprints_[index]; }

} // namespace node