#include "render/node.h"
#include "ui/networkview.h"

namespace ui {
namespace nodes {

class NodeParams : public QWidget {
  Q_OBJECT
public:
  NodeParams(Node &node, NetworkView &networkView);

  void rebuild();

private:
  Node &node_;
  NetworkView &networkView_;
  quint64 key_;
};

} // namespace nodes
} // namespace ui