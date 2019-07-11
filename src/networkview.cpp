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
//#include <QGraphicsL>

const double NODE_WIDTH = 150.0;
const double NODE_HEIGHT = 40.0;
const double NODE_LABEL_WIDTH = 200.0;
const double CONNECTOR_RADIUS = 7.0;

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
  /*connect(&scene_,
          SIGNAL(connectorClicked(NodeConnectorGraphicsObject*)),
          this, SLOT(connectorClicked(NodeConnectorGraphicsObject *)));*/
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
	disconnect(model_, SIGNAL(connectionAdded(const QModelIndex &, const QModelIndex &)),
		this,
		SLOT(connectionAddedPrivate(const QModelIndex &, const QModelIndex &)));
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
	connect(model_, SIGNAL(connectionAdded(const QModelIndex &, const QModelIndex &)),
		this,
		SLOT(connectionAddedPrivate(const QModelIndex &, const QModelIndex &)));
  }

  reset();
}

QAbstractItemModel *NetworkView::model() const {
  return model_ == staticEmptyModel() ? 0 : model_;
}

QList<QPersistentModelIndex> NetworkView::selectedNodes() const {
  auto selection = scene_.selectedItems();
  QList<QPersistentModelIndex> indices;
  for (auto s : selection) {
    if (auto obj = s->toGraphicsObject()) {
      if (auto node = qobject_cast<NodeGraphicsObject *>(obj)) {
        indices.push_back(node->nodeIndex_);
      }
    }
  }
  return indices;
}

void NetworkView::nodeAddedPrivate(int index) {
  qDebug() << "NetworkView::nodeAddedPrivate(" << index << ")";
  createNodeVisual(index);
}

void NetworkView::nodeRemovedPrivate(int index) {
  qDebug() << "NetworkView::nodeRemovedPrivate(" << index << ")";
  delete nodes[index];
  nodes.erase(nodes.begin() + index);
}

void NetworkView::nodeDeleteRequested(const QPersistentModelIndex &index) {
  auto i = index.row();
  model_->removeNode(i);
}

void NetworkView::inputConnectorAddedPrivate(const QModelIndex &parent,
                                             int index) {
  nodes[parent.row()]->inputConnectorAdded(index);
}

void NetworkView::outputConnectorAddedPrivate(const QModelIndex &parent,
                                              int index) {
  nodes[parent.row()]->outputConnectorAdded(index);
}

void NetworkView::inputConnectorRemovedPrivate(const QModelIndex &parent,
                                               int index) {
  nodes[parent.row()]->outputConnectorAdded(index);
}
void NetworkView::outputConnectorRemovedPrivate(const QModelIndex &parent,
                                                int index) {
  nodes[parent.row()]->outputConnectorRemoved(index);
}

void NetworkView::connectionAddedPrivate(const QModelIndex & fromConnector, const QModelIndex & toConnector)
{
	auto srcNode = nodes[fromConnector.parent().row()];
	auto srcConnector = srcNode->outputConnectors_[fromConnector.row()];
	auto dstNode = nodes[toConnector.parent().row()];
	auto dstConnector = dstNode->outputConnectors_[toConnector.row()];

	QPainterPath p;
	p.moveTo(srcConnector->scenePos());
	p.cubicTo(QPointF{ 0, -30 }, QPointF{ 0, 30 }, dstConnector->scenePos());
	auto connection = new QGraphicsPathItem{ p };
	scene_.addItem(connection);
	connections_.push_back(connection);
}

/*
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
*/

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
  auto n =
      new NodeGraphicsObject{QSizeF{NODE_WIDTH, NODE_HEIGHT},
                             QPersistentModelIndex{model_->nodeIndex(index)}};
  n->setPos(index * 300.0, 0.0);
  scene_.addItem(n);
  nodes.insert(nodes.begin() + index, n);
  connect(n, SIGNAL(deleteRequested(const QPersistentModelIndex &)), this,
          SLOT(nodeDeleteRequested(const QPersistentModelIndex &)));
  n->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

/*
void NetworkView::connectorClicked(NodeConnectorGraphicsObject *connector) {
  if (!makingConnection_) {
    makingConnection_ = true;
    connectionStart_ = connector->connectorIndex_;
    connectionStartType_ = connector->type_;
    connectionLine_ = new QGraphicsLineItem{QLineF{ connector->scenePos(), connector->scenePos()}};
    scene_.addItem(connectionLine_);
  } else {
    if (connector->type_ != connectionStartType_) {
      // endpoint types must be different
      qDebug() << "making connection (" << connectionStart_
               << " <-> " << connector->connectorIndex_ << ")";
      if (connectionStartType_ == AbstractNetworkModel::ConnectionType::Input) {
        Q_EMIT connectionAdded(connectionStart_, connector->connectorIndex_);
      } else {
        Q_EMIT connectionAdded(connector->connectorIndex_, connectionStart_);
      }
    }
    makingConnection_ = false;
    delete connectionLine_;
    connectionLine_ = nullptr;
  }
}*/

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

  // if not hitting a connector in the scene, then cancel connection
  bool resetConnection = true;

  // make connection line invisible so that it's not returned by itemAt
  if (connectionLine_) {
    connectionLine_->setVisible(false);
  }

  if (auto item = scene_.itemAt(mapToScene(e->pos()), QTransform{})) {
    if (auto c = qobject_cast<NodeConnectorGraphicsObject *>(
            item->toGraphicsObject())) {
      if (!makingConnection_) {
        makingConnection_ = true;
        connectionStart_ = c->connectorIndex_;
        connectionStartType_ = c->type_;
        connectionLine_ = new QGraphicsLineItem{
            QLineF{c->scenePos(), c->scenePos()}};
        scene_.addItem(connectionLine_);
        resetConnection = false;
      } else {
        if (c->type_ != connectionStartType_) {
          // endpoint types must be different
          qDebug() << "making connection (" << connectionStart_ << " <-> "
                   << c->connectorIndex_ << ")";
          if (connectionStartType_ ==
              AbstractNetworkModel::ConnectionType::Input) {
            Q_EMIT connectionRequest(connectionStart_,
                                   c->connectorIndex_);
          } else {
            Q_EMIT connectionRequest(c->connectorIndex_,
                                   connectionStart_);
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

NodeGraphicsObject::NodeGraphicsObject(QSizeF size,
                                       QPersistentModelIndex nodeIndex,
                                       QGraphicsItem *parent)
    : QGraphicsObject{parent}, size_{size}, nodeIndex_{nodeIndex} {
  setAcceptHoverEvents(true);
  inputConnectorsWidget_ = new QGraphicsWidget{this};
  outputConnectorsWidget_ = new QGraphicsWidget{this};
  inputConnectorsWidget_->setGeometry(0.0, -CONNECTOR_RADIUS * 1.5, 0.0, 0.0);
  outputConnectorsWidget_->setGeometry(
      0.0, NODE_HEIGHT + CONNECTOR_RADIUS * 1.5, 0.0, 0.0);

  /*auto label = new QGraphicsTextItem{"NODE", this};
  label->setFont(QFont{"Iosevka", 17});
  label->setPos(NODE_WIDTH + 4.0, 0.0);
  label->setFlag(QGraphicsItem::ItemIsSelectable, false);*/
}

QRectF NodeGraphicsObject::boundingRect() const {
  return QRectF{QPointF{0.0, 0.0},
                QSizeF{size_.width() + NODE_LABEL_WIDTH, size_.height()}};
}

QPainterPath NodeGraphicsObject::shape() const {
  QPainterPath p;
  p.addRect(QRectF{QPointF{0.0, 0.0}, size_});
  return p;
}

void NodeGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *ev) {
  // some bullshit
  // https://stackoverflow.com/questions/44757789/hover-on-a-child-qgraphicsitem-makes-parent-item-hovered-as-well
  if (contains(ev->pos())) {
    hover_ = true;
    update();
  }
}

void NodeGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *ev) {
  hover_ = false;
  update();
}

void NodeGraphicsObject::paint(QPainter *painter,
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

  // draw connectors
  /*for (int i = 0; i < numInputConnectors; ++i) {
    painter->drawEllipse(
        QPointF{(i + 1) * size_.width() / (numInputConnectors + 1), -11.0}, 7.0,
        7.0);
  }

  for (int i = 0; i < numOutputConnectors; ++i) {
    painter->drawEllipse(
        QPointF{(i + 1) * size_.width() / (numOutputConnectors + 1),
                size_.height() + 11.0},
        7.0, 7.0);
  }*/

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

void NodeGraphicsObject::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << "NodeGraphicsObject::dragEnterEvent(" << event << ")";
}

void NodeGraphicsObject::dropEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << "NodeGraphicsObject::dropEvent(" << event << ")";
}

void NodeGraphicsObject::contextMenuEvent(
    QGraphicsSceneContextMenuEvent *event) {
  QGraphicsObject::contextMenuEvent(event);
  /*QMenu contextMenu;
  auto deleteAction = contextMenu.addAction("Delete");
  if (contextMenu.exec(event->screenPos()) == deleteAction) {
    Q_EMIT deleteRequested(nodeIndex_);
  }*/
}

void NodeGraphicsObject::inputConnectorAdded(int index) {
  auto model = static_cast<const AbstractNetworkModel *>(nodeIndex_.model());

  auto item = new NodeConnectorGraphicsObject{
      model->inputConnector(nodeIndex_, index),
      AbstractNetworkModel::ConnectionType::Input, inputConnectorsWidget_};
  inputConnectors_.insert(index, item);
  updateConnectorLayout(inputConnectors_);
}

void NodeGraphicsObject::outputConnectorAdded(int index) {
  auto model = static_cast<const AbstractNetworkModel *>(nodeIndex_.model());
  auto item = new NodeConnectorGraphicsObject{
      model->outputConnector(nodeIndex_, index),
      AbstractNetworkModel::ConnectionType::Output, outputConnectorsWidget_};
  outputConnectors_.insert(index, item);
  updateConnectorLayout(outputConnectors_);
}

void NodeGraphicsObject::inputConnectorRemoved(int index) {
  delete inputConnectors_[index];
  inputConnectors_.removeAt(index);
}

void NodeGraphicsObject::outputConnectorRemoved(int index) {
  delete outputConnectors_[index];
  outputConnectors_.removeAt(index);
}

void NodeGraphicsObject::updateConnectorLayout(
    QList<NodeConnectorGraphicsObject *> &connectors) {
  auto n = connectors.size();
  for (int i = 0; i < n; ++i) {
    connectors[i]->setPos(QPointF{(i + 1) * size_.width() / (n + 1), 0.0});
  }
}

//=================================================================================
NodeConnectorGraphicsObject::NodeConnectorGraphicsObject(
    QPersistentModelIndex connectorIndex,
    AbstractNetworkModel::ConnectionType type, QGraphicsItem *parent)
    : QGraphicsObject{parent}, type_{type}, connectorIndex_{connectorIndex} {
  // setMaximumSize(QSizeF{2.0 * CONNECTOR_RADIUS, 2.0 * CONNECTOR_RADIUS});
  // setGeometry(-CONNECTOR_RADIUS, -CONNECTOR_RADIUS, 2.0 * CONNECTOR_RADIUS,
  //            2.0 * CONNECTOR_RADIUS);
  setAcceptHoverEvents(true);
  //setFlags(QGraphicsItem::ItemIsSelectable);
  //setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

QRectF NodeConnectorGraphicsObject::boundingRect() const {
  return QRectF{
      QPointF{-CONNECTOR_RADIUS, -CONNECTOR_RADIUS},
      QPointF{CONNECTOR_RADIUS, CONNECTOR_RADIUS},
  };
}

void NodeConnectorGraphicsObject::paint(QPainter *painter,
                                        const QStyleOptionGraphicsItem *option,
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

  /*if (hover_) {
    pen.setColor(Qt::blue);
  }*/

  painter->setPen(pen);
  painter->setBrush(hover_ ? Qt::blue : Qt::darkGray);

  painter->drawEllipse(boundingRect());
}

void NodeConnectorGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *e) {
  hover_ = true;
  update();
}

void NodeConnectorGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *e) {
  hover_ = false;
  update();
}

/*
void NodeConnectorGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *e) {
  e->accept();	// some bullshit (although documented)
}

void NodeConnectorGraphicsObject::mouseReleaseEvent(
    QGraphicsSceneMouseEvent *e) {
  auto s = static_cast<NetworkScene *>(scene());
  Q_EMIT s->connectorClicked(this);
  e->accept();
}
*/