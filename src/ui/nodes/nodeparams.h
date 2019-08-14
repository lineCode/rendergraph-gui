#include "render/node.h"
#include "ui/networkview.h"

namespace ui {
namespace nodes {

class NodeParams : public QWidget {
  Q_OBJECT
public:
  NodeParams(render::Node &node, NetworkView &networkView);

  void rebuild();

private:
	render::Node &node_;
  NetworkView &networkView_;
  quint64 key_;
};

} // namespace nodes
} // namespace ui