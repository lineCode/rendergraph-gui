#include "networkview.hpp"
#include "QtAwesome/QtAwesome.h"
#include <QAction>
#include <QDebug>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsTextItem>
#include <QLinearGradient>
#include <QMenu>
#include <QMouseEvent>
#include <QPainterPath>
#include <QScrollBar>
#include <QStyleOptionGraphicsItem>
#include <QWheelEvent>
#include <algorithm>

//=====================================================================================
// Node item metrics

const double NODE_WIDTH = 150.0;
const double NODE_HEIGHT = 40.0;
const double NODE_LABEL_WIDTH = 200.0;
const double CONNECTOR_RADIUS = 7.0;

//=====================================================================================
// EmptyNetworkModel

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
  QModelIndex nodeIndex(int index) const override { return {}; }
  int inputConnectorCount(const QModelIndex &parent) const override {
    return 0;
  };
  QModelIndex inputConnector(const QModelIndex &parent,
                             int index) const override {
    return {};
  };
  int outputConnectorCount(const QModelIndex &parent) const override {
    return 0;
  }
  QModelIndex outputConnector(const QModelIndex &parent,
                              int index) const override {
    return {};
  };
  int connectionCount(const QModelIndex &parent) const override { return 0; };
  QModelIndex connection(const QModelIndex &parent, int index) const override {
    return {};
  };
};

static EmptyNetworkModel *staticEmptyModel() {
  static EmptyNetworkModel m;
  return &m;
}

//=====================================================================================
// NodeGraphicsObjectPrivate

NodeGraphicsObjectPrivate::NodeGraphicsObjectPrivate(
    QSizeF size, QPersistentModelIndex nodeIndex, QGraphicsItem *parent)
    : QGraphicsObject{parent}, size_{size}, nodeIndex_{nodeIndex} {
  setAcceptHoverEvents(true);
  inputConnectorsWidget_ = new QGraphicsWidget{this};
  outputConnectorsWidget_ = new QGraphicsWidget{this};
  inputConnectorsWidget_->setGeometry(0.0, -CONNECTOR_RADIUS * 1.5, 0.0, 0.0);
  outputConnectorsWidget_->setGeometry(
      0.0, NODE_HEIGHT + CONNECTOR_RADIUS * 1.5, 0.0, 0.0);
  connect(this, SIGNAL(xChanged()), this, SLOT(positionChanged()));
  connect(this, SIGNAL(yChanged()), this, SLOT(positionChanged()));
  connect(this, SIGNAL(zChanged()), this, SLOT(positionChanged()));
}

void NodeGraphicsObjectPrivate::positionChanged() {
  auto s = static_cast<NetworkScene *>(scene());
  s->updateConnections();
}

QRectF NodeGraphicsObjectPrivate::boundingRect() const {
  return QRectF{QPointF{0.0, 0.0},
                QSizeF{size_.width() + NODE_LABEL_WIDTH, size_.height()}};
}

QPainterPath NodeGraphicsObjectPrivate::shape() const {
  QPainterPath p;
  p.addRect(QRectF{QPointF{0.0, 0.0}, size_});
  return p;
}

void NodeGraphicsObjectPrivate::hoverEnterEvent(QGraphicsSceneHoverEvent *ev) {
  // some bullshit
  // https://stackoverflow.com/questions/44757789/hover-on-a-child-qgraphicsitem-makes-parent-item-hovered-as-well
  if (contains(ev->pos())) {
    hover_ = true;
    update();
  }
}

void NodeGraphicsObjectPrivate::hoverLeaveEvent(QGraphicsSceneHoverEvent *ev) {
  hover_ = false;
  update();
}

void NodeGraphicsObjectPrivate::paint(QPainter *painter,
                                      const QStyleOptionGraphicsItem *option,
                                      QWidget *widget) {
  QPen pen;
  if (isSelected()) {
    pen.setWidthF(2.0);
    pen.setColor(Qt::yellow);
  } else {
    pen.setWidthF(0.0);
    pen.setColor(Qt::transparent);
  }

  painter->setPen(pen);
  painter->setBrush(hover_ ? Qt::gray : Qt::darkGray);
  painter->drawRoundedRect(QRectF{QPointF{0.0, 0.0}, size_}, 7.0, 7.0);

  auto model = static_cast<const AbstractNetworkModel *>(nodeIndex_.model());
  auto numInputConnectors = model->inputConnectorCount(nodeIndex_);
  auto numOutputConnectors = model->outputConnectorCount(nodeIndex_);

  auto bnd = boundingRect();
  QRectF textRegion{QPointF{size_.width() + 12.0, 0.0},
                    QPointF{bnd.right() - 20.0, bnd.bottom()}};
  // painter->setClipRect(clip);
  QFont font{"Iosevka", 17};

  if (isSelected()) {
    painter->setPen(Qt::yellow);
  } else {
    painter->setPen(hover_ ? Qt::gray : Qt::darkGray);
  }
  painter->setFont(font);
  QFontMetricsF metrics{font};
  QString elided =
      metrics.elidedText(QString::asprintf("NODE %i", nodeIndex_.row()),
                         Qt::ElideRight, textRegion.width());
  QTextOption opt{Qt::AlignVCenter};
  opt.setWrapMode(QTextOption::NoWrap);
  painter->drawText(textRegion, elided, opt);
}

void NodeGraphicsObjectPrivate::dragEnterEvent(
    QGraphicsSceneDragDropEvent *event) {
  qDebug() << "NodeGraphicsObjectPrivate::dragEnterEvent(" << event << ")";
}

void NodeGraphicsObjectPrivate::dropEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << "NodeGraphicsObjectPrivate::dropEvent(" << event << ")";
}

void NodeGraphicsObjectPrivate::contextMenuEvent(
    QGraphicsSceneContextMenuEvent *event) {
  QGraphicsObject::contextMenuEvent(event);
}

void NodeGraphicsObjectPrivate::inputConnectorAdded(int index) {
  auto model = static_cast<const AbstractNetworkModel *>(nodeIndex_.model());
  auto item = new NodeConnectorGraphicsObjectPrivate{
      model->inputConnector(nodeIndex_, index),
      AbstractNetworkModel::ConnectionType::Input, inputConnectorsWidget_};
  inputConnectors_.insert(index, item);
  updateConnectorLayout(inputConnectors_);
}

void NodeGraphicsObjectPrivate::outputConnectorAdded(int index) {
  auto model = static_cast<const AbstractNetworkModel *>(nodeIndex_.model());
  auto item = new NodeConnectorGraphicsObjectPrivate{
      model->outputConnector(nodeIndex_, index),
      AbstractNetworkModel::ConnectionType::Output, outputConnectorsWidget_};
  outputConnectors_.insert(index, item);
  updateConnectorLayout(outputConnectors_);
}

void NodeGraphicsObjectPrivate::inputConnectorRemoved(int index) {
  delete inputConnectors_[index];
  inputConnectors_.removeAt(index);
}

void NodeGraphicsObjectPrivate::outputConnectorRemoved(int index) {
  delete outputConnectors_[index];
  outputConnectors_.removeAt(index);
}

void NodeGraphicsObjectPrivate::updateConnectorLayout(
    QList<NodeConnectorGraphicsObjectPrivate *> &connectors) {
  auto n = connectors.size();
  for (int i = 0; i < n; ++i) {
    connectors[i]->setPos(QPointF{(i + 1) * size_.width() / (n + 1), 0.0});
  }
}

//=====================================================================================
// NodeConnectionGraphicsItemPrivate
void NodeConnectionGraphicsItemPrivate::updatePositions() {
  QPainterPath p;
  auto srcPos = srcConn_->scenePos();
  auto dstPos = dstConn_->scenePos();
  p.moveTo(srcPos);
  p.cubicTo(srcPos + QPointF{0, 30}, dstPos + QPointF{0, -30}, dstPos);
  setPath(p);
}

//=====================================================================================
// NodeConnectorGraphicsObjectPrivate
NodeConnectorGraphicsObjectPrivate::NodeConnectorGraphicsObjectPrivate(
    QPersistentModelIndex connectorIndex,
    AbstractNetworkModel::ConnectionType type, QGraphicsItem *parent)
    : QGraphicsObject{parent}, type_{type}, connectorIndex_{connectorIndex} {
  setAcceptHoverEvents(true);
}

QRectF NodeConnectorGraphicsObjectPrivate::boundingRect() const {
  return QRectF{
      QPointF{-CONNECTOR_RADIUS, -CONNECTOR_RADIUS},
      QPointF{CONNECTOR_RADIUS, CONNECTOR_RADIUS},
  };
}

void NodeConnectorGraphicsObjectPrivate::paint(
    QPainter *painter, const QStyleOptionGraphicsItem *option,
    QWidget *widget) {
  // grandparent is node
  bool selected = isSelected() || parentItem()->isSelected() ||
                  parentItem()->parentItem()->isSelected();
  QPen pen;
  if (selected) {
    pen.setWidthF(2.0);
    pen.setColor(Qt::yellow);
  } else {
    pen.setWidthF(0.0);
    pen.setColor(Qt::transparent);
  }

  painter->setPen(pen);
  painter->setBrush(hover_ ? Qt::blue : Qt::darkGray);
  painter->drawEllipse(boundingRect());
}

void NodeConnectorGraphicsObjectPrivate::hoverEnterEvent(
    QGraphicsSceneHoverEvent *e) {
  hover_ = true;
  update();
}

void NodeConnectorGraphicsObjectPrivate::hoverLeaveEvent(
    QGraphicsSceneHoverEvent *e) {
  hover_ = false;
  update();
}

//=====================================================================================
// NetworkScene

NetworkScene::NetworkScene(QObject *parent)
    : QGraphicsScene{parent}, model_{staticEmptyModel()} {}

void NetworkScene::setModel(AbstractNetworkModel *model) {
  auto empty = staticEmptyModel();
  if (model == model_) { // no change
    return;
  }

  if (model && model_ != empty) {
    disconnect(model_, SIGNAL(nodeAdded(int)), this,
               SLOT(nodeAddedPrivate(int)));
    disconnect(model_, SIGNAL(nodeRemoved(int)), this,
               SLOT(nodeRemovedPrivate(int)));
    disconnect(model_, SIGNAL(inputConnectorAdded(const QModelIndex &, int)),
               this,
               SLOT(inputConnectorAddedPrivate(const QModelIndex &, int)));
    disconnect(model_, SIGNAL(inputConnectorRemoved(const QModelIndex &, int)),
               this,
               SLOT(inputConnectorRemovedPrivate(const QModelIndex &, int)));
    disconnect(model_, SIGNAL(outputConnectorAdded(const QModelIndex &, int)),
               this,
               SLOT(outputConnectorAddedPrivate(const QModelIndex &, int)));
    disconnect(model_, SIGNAL(outputConnectorRemoved(const QModelIndex &, int)),
               this,
               SLOT(outputConnectorRemovedPrivate(const QModelIndex &, int)));
    disconnect(
        model_,
        SIGNAL(connectionAdded(const QModelIndex &, const QModelIndex &)), this,
        SLOT(connectionAddedPrivate(const QModelIndex &, const QModelIndex &)));
    disconnect(
        model_,
        SIGNAL(connectionRemoved(const QModelIndex &, const QModelIndex &)),
        this,
        SLOT(connectionRemovedPrivate(const QModelIndex &,
                                      const QModelIndex &)));
  }

  model_ = (model == nullptr) ? empty : model;

  if (model) {
    connect(model_, SIGNAL(nodeAdded(int)), this, SLOT(nodeAddedPrivate(int)));
    connect(model_, SIGNAL(nodeRemoved(int)), this,
            SLOT(nodeRemovedPrivate(int)));
    connect(model_, SIGNAL(inputConnectorAdded(const QModelIndex &, int)), this,
            SLOT(inputConnectorAddedPrivate(const QModelIndex &, int)));
    connect(model_, SIGNAL(inputConnectorRemoved(const QModelIndex &, int)),
            this, SLOT(inputConnectorRemovedPrivate(const QModelIndex &, int)));
    connect(model_, SIGNAL(outputConnectorAdded(const QModelIndex &, int)),
            this, SLOT(outputConnectorAddedPrivate(const QModelIndex &, int)));
    connect(model_, SIGNAL(outputConnectorRemoved(const QModelIndex &, int)),
            this,
            SLOT(outputConnectorRemovedPrivate(const QModelIndex &, int)));
    connect(
        model_,
        SIGNAL(connectionAdded(const QModelIndex &, const QModelIndex &)), this,
        SLOT(connectionAddedPrivate(const QModelIndex &, const QModelIndex &)));
    connect(model_,
            SIGNAL(connectionRemoved(const QModelIndex &, const QModelIndex &)),
            this,
            SLOT(connectionRemovedPrivate(const QModelIndex &,
                                          const QModelIndex &)));
  }

  // reset();
}

QAbstractItemModel *NetworkScene::model() const {
  return model_ == staticEmptyModel() ? 0 : model_;
}

QList<QPersistentModelIndex> NetworkScene::selectedNodes() const {
  auto selection = selectedItems();
  QList<QPersistentModelIndex> indices;
  for (auto s : selection) {
    if (auto obj = s->toGraphicsObject()) {
      if (auto node = qobject_cast<NodeGraphicsObjectPrivate *>(obj)) {
        indices.push_back(node->nodeIndex_);
      }
    }
  }
  return indices;
}

void NetworkScene::nodeAddedPrivate(int index) {
  qDebug() << "NetworkView::nodeAddedPrivate(" << index << ")";
  createNodeVisual(index);
}

void NetworkScene::nodeRemovedPrivate(int index) {
  qDebug() << "NetworkView::nodeRemovedPrivate(" << index << ")";
  delete nodes[index];
  // connections will be removed before
  nodes.erase(nodes.begin() + index);
}

void NetworkScene::nodeDeleteRequested(const QPersistentModelIndex &index) {
  auto i = index.row();
  model_->removeNode(i);
}

void NetworkScene::inputConnectorAddedPrivate(const QModelIndex &parent,
                                              int index) {
  nodes[parent.row()]->inputConnectorAdded(index);
}

void NetworkScene::outputConnectorAddedPrivate(const QModelIndex &parent,
                                               int index) {
  nodes[parent.row()]->outputConnectorAdded(index);
}

void NetworkScene::inputConnectorRemovedPrivate(const QModelIndex &parent,
                                                int index) {
  nodes[parent.row()]->outputConnectorAdded(index);
}
void NetworkScene::outputConnectorRemovedPrivate(const QModelIndex &parent,
                                                 int index) {
  nodes[parent.row()]->outputConnectorRemoved(index);
}

void NetworkScene::connectionAddedPrivate(const QModelIndex &fromConnector,
                                          const QModelIndex &toConnector) {
  auto srcNode = nodes[fromConnector.parent().row()];
  auto srcConnector = srcNode->outputConnectors_[fromConnector.row()];
  auto dstNode = nodes[toConnector.parent().row()];
  auto dstConnector = dstNode->inputConnectors_[toConnector.row()];

  auto connection =
      new NodeConnectionGraphicsItemPrivate{srcConnector, dstConnector};
  connection->setPen(QPen{Qt::black, 2.0});
  addItem(connection);
  connections_.push_back(connection);
  connection->updatePositions();
}

void NetworkScene::connectionRemovedPrivate(const QModelIndex &fromConnector,
                                            const QModelIndex &toConnector) {
  connections_.erase(
      std::remove_if(connections_.begin(), connections_.end(),
                     [=](NodeConnectionGraphicsItemPrivate *item) {
                       auto b =
                           item->srcConn_->connectorIndex_ == fromConnector &&
                           item->dstConn_->connectorIndex_ == toConnector;
                       // qDebug() << "from=" << fromConnector << " to=" <<
                       // toConnector << " src=" <<
                       // item->srcConn_->connectorIndex_ << " dest=" <<
                       // item->dstConn_->connectorIndex_;
                       if (b) {
                         delete item;
                         // qDebug() << "removing connection item" << item;
                       }
                       return b;
                     }),
      connections_.end());
}

void NetworkScene::createNodeVisual(int index) {
  auto n = new NodeGraphicsObjectPrivate{
      QSizeF{NODE_WIDTH, NODE_HEIGHT},
      QPersistentModelIndex{model_->nodeIndex(index)}};
  n->setPos(index * 300.0, 0.0);
  addItem(n);
  nodes.insert(nodes.begin() + index, n);
  connect(n, SIGNAL(deleteRequested(const QPersistentModelIndex &)), this,
          SLOT(nodeDeleteRequested(const QPersistentModelIndex &)));
  n->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void NetworkScene::updateConnections() {
  for (auto c : connections_) {
    c->updatePositions();
  }
}

//=====================================================================================
// NetworkView

NetworkView::NetworkView(QWidget *parent) : QGraphicsView{parent} {
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

void NetworkView::reset() {
  // TODO
}

void NetworkView::mousePressEvent(QMouseEvent *e) {
  // bool alt = e->modifiers() & Qt::AltModifier;
  if (e->buttons() & Qt::MiddleButton) {
    mouseAnchor = e->pos();
    translationBeforeDrag =
        QPoint{horizontalScrollBar()->value(), verticalScrollBar()->value()};
  }

  // if not hitting a connector in the scene, then cancel connection
  bool resetConnection = true;

  // make connection line invisible so that it's not returned by itemAt
  if (connectionLine_) {
    connectionLine_->setVisible(false);
  }

  if (auto item = scene_.itemAt(mapToScene(e->pos()), QTransform{})) {
    if (auto c = qobject_cast<NodeConnectorGraphicsObjectPrivate *>(
            item->toGraphicsObject())) {
      if (!makingConnection_) {
        makingConnection_ = true;
        connectionStart_ = c;
        connectionLine_ =
            new QGraphicsLineItem{QLineF{c->scenePos(), c->scenePos()}};
        scene_.addItem(connectionLine_);
        resetConnection = false;
      } else {
        if (c->type_ != connectionStart_->type_) {
          // endpoint types must be different
          qDebug() << "making connection (" << connectionStart_ << " <-> "
                   << c->connectorIndex_ << ")";
          if (connectionStart_->type_ ==
              AbstractNetworkModel::ConnectionType::Output) {
            Q_EMIT connectionRequest(connectionStart_->connectorIndex_,
                                     c->connectorIndex_);
          } else {
            Q_EMIT connectionRequest(c->connectorIndex_,
                                     connectionStart_->connectorIndex_);
          }
        }
      }
    }
  }

  if (resetConnection) {
    makingConnection_ = false;
    delete connectionLine_;
    connectionLine_ = nullptr;
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
  if (connectionLine_) {
    connectionLine_->setLine(
        QLineF{connectionLine_->line().p1(), mapToScene(e->pos())});
  }
  QGraphicsView::mouseMoveEvent(e);
}

void NetworkView::wheelEvent(QWheelEvent *e) {
  qreal detail =
      QStyleOptionGraphicsItem::levelOfDetailFromTransform(transform());

  if (e->delta() > 0 && detail < 8.0) {
    scale(2.0, 2.0);
  } else if (e->delta() < 0 && detail > (1.0 / 8.0)) {
    scale(0.5, 0.5);
  }
}
