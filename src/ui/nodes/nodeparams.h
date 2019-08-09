#include "render/node.h"
#include "ui/networkview.h"

namespace ui {
namespace nodes {

class NodeParams : public QObject, public render::Node {
  Q_OBJECT
public:
  using Ptr = std::unique_ptr<NodeParams>;

  NodeParams(NetworkView &networkView);

  void rebuildParamUI(QWidget *parent);

  static NodeParams *make(render::Node *parent,
	  NetworkView &networkView);

private:
  NetworkView &networkView_;
  QWidget *panel_ = nullptr;
  quint64 key_;
};

} // namespace nodes
} // namespace ui