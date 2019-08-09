#include "ui/nodes/nodeparams.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QWidget>

namespace ui {
namespace nodes {

NodeParams::NodeParams(NetworkView &networkView)
    : render::Node{"editor"}, networkView_{ networkView }
{
}

void NodeParams::rebuildParamUI(QWidget *parentWidget) {

  // create the user interface of the node
  Node *p = Node::parent();

  auto params = p->findChildrenByType<render::Param>();

  QFormLayout *layout = new QFormLayout{};
  parentWidget->setLayout(layout);

  for (auto &&param : params) {
    auto name = param->name();
    auto spinBox = new QDoubleSpinBox{};
    spinBox->setMinimum(0.0);
    spinBox->setMaximum(1.0);

    // TODO: should be a child node of the parameter so that it's deleted along
    // with the parameter
    connect(spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
            [param](double newValue) { param->setValue(newValue); });

    layout->addRow(QString::fromUtf8(name.ptr, name.len), spinBox);
  }
}

NodeParams *NodeParams::make(render::Node *parent,
	NetworkView &networkView) {
  auto nodeParams = std::make_unique<NodeParams>(networkView);
  return static_cast<NodeParams *>(parent->addChild(std::move(nodeParams)));
}

} // namespace nodes
} // namespace ui