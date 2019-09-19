#include "template.h"
#include "util/log.h"

namespace node {
//----------------------------------------------------------------------------------
void TemplateTable::registerTemplate(std::unique_ptr<NodeTemplate> tpl) {
  // check for duplicate name
  if (findTemplate(tpl->name_)) {
    util::log("WARNING TemplateTable::registerTemplate: duplicate templates with "
              "name `{}`",
              tpl->name_);
    return;
  }
  templates_.push_back(std::move(tpl));
}

NodeTemplate *TemplateTable::findTemplate(util::StringRef name) {
  for (auto &&tpl : templates_) {
    if (tpl->name_ == name) {
      return tpl.get();
    }
  }
  return nullptr;
}

int TemplateTable::count() { return (int)templates_.size(); }
NodeTemplate &TemplateTable::at(int index) { return *templates_[index]; }

} // namespace node