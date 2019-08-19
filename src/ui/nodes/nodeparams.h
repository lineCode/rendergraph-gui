#include "node/node.h"
#include "ui/networkview.h"

namespace ui {

class NodeParams : public QWidget {
  Q_OBJECT
public:
  NodeParams(::node::Node &node, NetworkView &networkView);

  void rebuild();

private:
  ::node::Node &node_;
  NetworkView &networkView_;
  quint64 key_;
};

} // namespace ui