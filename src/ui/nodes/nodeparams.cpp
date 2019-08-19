#include "ui/nodes/nodeparams.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QWidget>

using node::Node;

namespace ui {

NodeParams::NodeParams(Node &node, NetworkView &networkView)
    : node_{node}, networkView_{networkView} {}

void NodeParams::rebuild() {

  QFormLayout *layout = new QFormLayout{};
  setLayout(layout);

  int np = node_.paramCount();
  for (int i = 0; i < np; ++i) {
    auto param = node_.param(i);
    auto name = param->name();
    auto spinBox = new QDoubleSpinBox{};
    spinBox->setMinimum(0.0);
    spinBox->setMaximum(1.0);

    // TODO: should be a child node of the parameter so that it's deleted along
    // with the parameter
    connect(spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
            [param](double newValue) { param->setValue(newValue); });

    layout->addRow(QString::fromUtf8(name.data(), (int)name.size()), spinBox);
  }
}

} // namespace ui