#include "networkview.hpp"
#include <QAction>
#include <QDebug>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneContextMenuEvent>
#include <QLinearGradient>
#include <QMenu>
#include <QMouseEvent>
#include <QPainterPath>
#include <QScrollBar>
#include <QWheelEvent>
#include <algorithm>
//#include <QGraphicsL>

class EmptyNetworkModel : public AbstractNetworkModel {
public:
  explicit EmptyNetworkModel(QObject *parent = 0)
      : AbstractNetworkModel(parent) {}

  QModelIndex index(int, int, const QModelIndex &) const override {
    return QModelIndex();
  }
  QModelIndex parent(const QModelIndex &) const override {
    return QModelIndex();
  }
  int rowCount(const QModelIndex &) const override { return 0; }
  int columnCount(const QModelIndex &) const override { return 0; }
  bool hasChildren(const QModelIndex &) const override { return false; }
  QVariant data(const QModelIndex &, int) const override { return QVariant(); }

  int nodeCount() const override { return 0; }
  NodeIndex node(int index) const override { return {}; }
  NodeIndex nodeByID(NodeID id) const override { return {}; }
  int inputConnectorCount(const NodeIndex &parent) const override { return 0; };
  ConnectorIndex inputConnector(const NodeIndex &parent,
                                int index) const override {
    return {};
  };
  ConnectorIndex inputConnectorByID(const NodeIndex &parent,
                                    ConnectorID connector) const override {
    return {};
  }
  int outputConnectorCount(const NodeIndex &parent) const override { return 0; }
  ConnectorIndex outputConnector(const NodeIndex &parent,
                                 int index) const override {
    return {};
  };
  ConnectorIndex outputConnectorByID(const NodeIndex &parent,
                                     ConnectorID connector) const override {
    return {};
  };
  int inputConnectionCount(const NodeIndex &parent) const override {
    return 0;
  };
  ConnectionIndex inputConnection(const NodeIndex &parent,
                                  int index) const override {
    return {};
  };
  int outputConnectionCount(const NodeIndex &parent) const override {
    return 0;
  };
  ConnectionIndex outputConnection(const NodeIndex &parent,
                                   int index) const override {
    return {};
  };
  Endpoints endpoints(const ConnectionIndex &connection) const override {
    return {};
  };
};

static EmptyNetworkModel *staticEmptyModel() {
  static EmptyNetworkModel m;
  return &m;
}

NetworkView::NetworkView(QWidget *parent)
    : QGraphicsView{parent}, model_{staticEmptyModel()} {
  setRenderHints(QPainter::Antialiasing);
  setSceneRect(-6000.0, -6000.0, 12000.0, 12000.0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // Scrolling is implemented manually, using the middle mouse button
  setDragMode(QGraphicsView::RubberBandDrag);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  // setContextMenuPolicy(Qt::CustomContextMenu);
  setScene(&scene_);
}

NetworkView::~NetworkView() {}

void NetworkView::setModel(AbstractNetworkModel *model) {
  auto empty = staticEmptyModel();
  if (model == model_) { // no change
    return;
  }

  if (model && model_ != empty) {
    disconnect(model_, SIGNAL(nodeAdded(int)), this,
               SLOT(nodeAddedPrivate(int)));
    disconnect(model_, SIGNAL(nodeRemoved(int)), this,
               SLOT(nodeRemovedPrivate(int)));
  }

  model_ = (model == nullptr) ? empty : model;

  if (model) {
    connect(model_, SIGNAL(nodeAdded(int)), this, SLOT(nodeAddedPrivate(int)));
    connect(model_, SIGNAL(nodeRemoved(int)), this,
            SLOT(nodeRemovedPrivate(int)));
  }

  reset();
}

QAbstractItemModel *NetworkView::model() const {
  return model_ == staticEmptyModel() ? 0 : model_;
}

void NetworkView::nodeAddedPrivate(int index) {
  qDebug() << "NetworkView::nodeAddedPrivate(" << index << ")";
  createNodeVisual(index);
}

void NetworkView::nodeRemovedPrivate(int index) {
  qDebug() << "NetworkView::nodeRemovedPrivate(" << index << ")";
}

void NetworkView::connectorAddedPrivate(
    const AbstractNetworkModel::NodeIndex &parent,
    AbstractNetworkModel::ConnectionType type, int index) {
  qDebug() << "NetworkView::connectorAddedPrivate(" << parent.index << ","
           << (int)type << "," << index << ")";
}

void NetworkView::connectorRemovedPrivate(
    const AbstractNetworkModel::NodeIndex &parent,
    AbstractNetworkModel::ConnectionType type, int index) {
  qDebug() << "NetworkView::connectorRemovedPrivate(" << parent.index << ","
           << (int)type << "," << index << ")";
}

void NetworkView::connectionAddedPrivate(
    const AbstractNetworkModel::NodeIndex &parent,
    AbstractNetworkModel::ConnectionType type, int index) {
  qDebug() << "NetworkView::connectionAddedPrivate(" << parent.index << ","
           << (int)type << "," << index << ")";
}

void NetworkView::connectionRemovedPrivate(
    const AbstractNetworkModel::NodeIndex &parent,
    AbstractNetworkModel::ConnectionType type, int index) {
  qDebug() << "NetworkView::connectionRemovedPrivate(" << parent.index << ","
           << (int)type << "," << index << ")";
}

void NetworkView::contextMenuEvent(QContextMenuEvent *e) {
  if (itemAt(e->pos())) {
    QGraphicsView::contextMenuEvent(e);
    return;
  }

  QMenu contextMenu;
  auto deleteAction = contextMenu.addAction("Add node");
  contextMenu.exec(e->globalPos());
}

void NetworkView::drawBackground(QPainter *painter, const QRectF &rect) {
  QPen pen{Qt::black, 1.0};
  pen.setWidth(12);
  const double gridStep = 50.0;

  double left = std::ceil(rect.left() / gridStep) * gridStep;
  double right = rect.right();
  double top = std::ceil(rect.top() / gridStep) * gridStep;
  double bottom = rect.bottom();

  painter->setPen(Qt::lightGray);
  for (double h = left + 0.5; h < right; h += gridStep) {
    painter->drawLine(QLineF{h, rect.top(), h, rect.bottom()});
  }
  for (double v = top + 0.5; v < bottom; v += gridStep) {
    painter->drawLine(QLineF{rect.left(), v, rect.right(), v});
  }
}

void NetworkView::reset() {}

void NetworkView::createNodeVisual(int index) {
  auto n = new NodeGraphicsObject{QSizeF{150, 40}};
  n->setPos(index * 300.0, 0.0);
  // auto text = new QGraphicsTextItem{ QString::asprintf("Node %i", index), n
  // };
  scene_.addItem(n);
  n->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

AbstractNetworkModel::AbstractNetworkModel(QObject *parent)
    : QAbstractItemModel{parent} {}

AbstractNetworkModel::~AbstractNetworkModel() {}

void NetworkView::mousePressEvent(QMouseEvent *e) {
  // bool alt = e->modifiers() & Qt::AltModifier;
  if (e->buttons() & Qt::MiddleButton) {
    mouseAnchor = e->pos();
    translationBeforeDrag =
        QPoint{horizontalScrollBar()->value(), verticalScrollBar()->value()};
  }
  QGraphicsView::mousePressEvent(e);
}

void NetworkView::mouseMoveEvent(QMouseEvent *e) {
  if (e->buttons() & Qt::MiddleButton) {
    auto d = QPointF{mouseAnchor - e->pos()};
    setTransformationAnchor(QGraphicsView::NoAnchor);
    horizontalScrollBar()->setValue(translationBeforeDrag.x() + d.x());
    verticalScrollBar()->setValue(translationBeforeDrag.y() + d.y());
  }
  QGraphicsView::mouseMoveEvent(e);
}

void NetworkView::wheelEvent(QWheelEvent *e) {
  if (e->delta() > 0) {
    scale(2.0, 2.0);
  } else {
    scale(0.5, 0.5);
  }
}

NodeGraphicsObject::NodeGraphicsObject(QSizeF size, QGraphicsItem *parent)
    : QGraphicsObject{parent}, size_{size} {}

QRectF NodeGraphicsObject::boundingRect() const {
  return QRectF{QPointF{0.0, 0.0}, size_};
}

void NodeGraphicsObject::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
  QPen pen;
  if (isSelected()) {
    pen.setColor(Qt::yellow);
  } else {
    pen.setColor(Qt::black);
  }
  pen.setWidthF(2.0);
  QLinearGradient gradient{QPointF{0.0, 0.0}, QPointF{0.0, size_.height()}};
  gradient.setColorAt(0.0, Qt::darkGray);
  gradient.setColorAt(1.0, Qt::lightGray);
  painter->setPen(pen);
  painter->setBrush(gradient);
  painter->drawRoundedRect(boundingRect(), 20.0, 20.0);

  QRectF clip{QPointF{20.0, 0.0},
              QPointF{size_.width() - 20.0, size_.height()}};
  painter->setClipRect(clip);
  QTextOption opt{Qt::AlignVCenter};
  opt.setWrapMode(QTextOption::NoWrap);
  QFont font;
  font.setPointSize(16);
  painter->setFont(font);
  QFontMetricsF metrics{font};
  QString elided = metrics.elidedText("NODE! blahblahblahblahblah",
                                      Qt::ElideRight, clip.width());
  painter->drawText(clip, elided, opt);
}

void NodeGraphicsObject::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << "NodeGraphicsObject::dragEnterEvent(" << event << ")";
}

void NodeGraphicsObject::dropEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << "NodeGraphicsObject::dropEvent(" << event << ")";
}

void NodeGraphicsObject::contextMenuEvent(
    QGraphicsSceneContextMenuEvent *event) {
  QMenu contextMenu;
  auto deleteAction = contextMenu.addAction("Delete");
  contextMenu.exec(event->screenPos());
}
