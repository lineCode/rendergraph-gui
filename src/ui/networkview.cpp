#include "networkview.h"
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

#include "render/output.h"
#include "render/input.h"

using render::Input;
using render::Node;
using render::Output;

//=====================================================================================
// Node item metrics

const double NODE_WIDTH = 150.0;
const double NODE_HEIGHT = 40.0;
const double NODE_LABEL_WIDTH = 200.0;
const double CONNECTOR_RADIUS = 7.0;

//=====================================================================================
// NodeGraphicsObjectPrivate

// Constructor
NodeGraphicsObjectPrivate::NodeGraphicsObjectPrivate(QSizeF size, Node *node,
                                                     QGraphicsItem *parent)
    : QGraphicsObject{parent}, size_{size}, node_{node} {
  nodeObserver_ =
      render::Observer::make(node, [this](const render::EventData &e) {
        switch (e.type) {
        case render::EventType::InputAdded:
          this->inputConnectorAdded(e.u.inputAdded.index);
          break;
        case render::EventType::InputRemoved:
          this->inputConnectorRemoved(e.u.inputRemoved.index);
          break;
        case render::EventType::OutputAdded:
          this->outputConnectorAdded(e.u.outputAdded.index);
          break;
        case render::EventType::OutputRemoved:
          this->outputConnectorRemoved(e.u.outputRemoved.index);
          break;
        default:
          break;
        }
      });
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

  auto numInputConnectors = 1;  // TODO
  auto numOutputConnectors = 1; // TODO

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
  auto nodeName = node_->name();
  QString elided =
      metrics.elidedText(QString::fromUtf8(nodeName.data(), nodeName.size()),
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
  auto item = new InputConnectorGraphicsObjectPrivate{node_->input(index),
                                                      inputConnectorsWidget_};

  inputConnectors_.insert(index, item);
  updateConnectorLayout(inputConnectors_);
}

void NodeGraphicsObjectPrivate::outputConnectorAdded(int index) {
  auto item = new OutputConnectorGraphicsObjectPrivate{node_->output(index),
                                                       outputConnectorsWidget_};
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
    QList<ConnectorGraphicsObjectPrivate *> &connectors) {
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
  srcPos.setY(srcPos.y() + CONNECTOR_RADIUS);
  auto dstPos = dstConn_->scenePos();
  dstPos.setY(dstPos.y() - CONNECTOR_RADIUS);
  p.moveTo(srcPos);
  p.cubicTo(srcPos + QPointF{0, 30}, dstPos + QPointF{0, -30}, dstPos);
  setPath(p);
}

//=====================================================================================
// NodeConnectorGraphicsObjectPrivate
ConnectorGraphicsObjectPrivate::ConnectorGraphicsObjectPrivate(
    QGraphicsItem *parent)
    : QGraphicsObject{parent} {
  setAcceptHoverEvents(true);
}

QRectF ConnectorGraphicsObjectPrivate::boundingRect() const {
  return QRectF{
      QPointF{-CONNECTOR_RADIUS, -CONNECTOR_RADIUS},
      QPointF{CONNECTOR_RADIUS, CONNECTOR_RADIUS},
  };
}

void ConnectorGraphicsObjectPrivate::paint(
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

void ConnectorGraphicsObjectPrivate::hoverEnterEvent(
    QGraphicsSceneHoverEvent *e) {
  hover_ = true;
  update();
}

void ConnectorGraphicsObjectPrivate::hoverLeaveEvent(
    QGraphicsSceneHoverEvent *e) {
  hover_ = false;
  update();
}

//=====================================================================================
// NetworkScene

NetworkScene::NetworkScene(QObject *parent)
    : QGraphicsScene{parent}, network_{nullptr} {}

void NetworkScene::setNetwork(Node *network) {
  // whatever
  // reset();
  network_ = network;
  networkObserver_ =
      render::Observer::make(network_, [this](const render::EventData &e) {
        switch (e.type) {
        case render::EventType::ChildAdded:
          nodeAdded(e.u.childAdded.node);
          break;
        case render::EventType::ChildRemoved:
          nodeRemoved(e.u.childRemoved.node);
          break;
        case render::EventType::NodeDeleted:
        default:
          break;
        }
      });
}

Node *NetworkScene::network() const { return network_; }

QVector<Node *> NetworkScene::selectedNodes() const {
  auto selection = selectedItems();
  QVector<Node *> nodes;
  for (auto s : selection) {
    if (auto obj = s->toGraphicsObject()) {
      if (auto node = qobject_cast<NodeGraphicsObjectPrivate *>(obj)) {
        nodes.push_back(node->node_);
      }
    }
  }
  return nodes;
}

void NetworkScene::nodeAdded(Node *node) {
  qDebug() << "NetworkView::nodeAddedPrivate(" << node << ")";
  createNodeVisual(node);
}

void NetworkScene::nodeRemoved(Node *node) {
  qDebug() << "NetworkView::nodeRemovedPrivate(" << node << ")";
  delete nodes_[node];
  // connections will be removed before
  nodes_.erase(node);
}

/*
void NetworkScene::nodeDeleteRequested(const Node* node) {
  auto i = index.row();
  model_->removeNode(i);
}*/

void NetworkScene::inputConnectorAdded(Node *node, int index) {
  nodes_[node]->inputConnectorAdded(index);
}

void NetworkScene::outputConnectorAdded(Node *node, int index) {
  nodes_[node]->outputConnectorAdded(index);
}

void NetworkScene::inputConnectorRemoved(Node *node, int index) {
  nodes_[node]->outputConnectorAdded(index);
}
void NetworkScene::outputConnectorRemoved(Node *node, int index) {
  nodes_[node]->outputConnectorRemoved(index);
}

void NetworkScene::connectionAdded(Output *from, Input *to) {
  auto srcNode = nodes_[from->owner()];
  auto dstNode = nodes_[to->owner()];

  auto srcConn = srcNode->findOutputConnector(from);
  auto dstConn = dstNode->findInputConnector(to);

  auto connection =
      new NodeConnectionGraphicsItemPrivate{ srcConn, dstConn };
  connection->setPen(QPen{Qt::black, 2.0});
  addItem(connection);
  connections_.push_back(connection);
  connection->updatePositions();
}

void NetworkScene::connectionRemoved(Output *from, Input *to) {
  connections_.erase(
      std::remove_if(connections_.begin(), connections_.end(),
                     [=](NodeConnectionGraphicsItemPrivate *item) {
                       auto b = item->srcConn_->output_ == from &&
                                item->dstConn_->input_ == to;
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

void NetworkScene::createNodeVisual(Node *node) {
  auto n = new NodeGraphicsObjectPrivate{QSizeF{NODE_WIDTH, NODE_HEIGHT}, node};
  n->setPos(0.0, 0.0); // TODO
  addItem(n);
  nodes_.insert({node, n});
  // connect(n, &NodeGraphicsObjectPrivate::deleteRequested, this,
  // &NetworkScene::nodeD		// TODO
  //        SLOT(nodeDeleteRequested(const QPersistentModelIndex &)));
  n->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

void NetworkScene::updateConnections() {
  for (auto c : connections_) {
    c->updatePositions();
  }
}

//=====================================================================================
// NetworkView

NetworkView::NetworkView(Node *network, QWidget *parent)
    : QGraphicsView{parent} {
  setRenderHints(QPainter::Antialiasing);
  setSceneRect(-6000.0, -6000.0, 12000.0, 12000.0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  // Scrolling is implemented manually, using the middle mouse button
  setDragMode(QGraphicsView::RubberBandDrag);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  // setContextMenuPolicy(Qt::CustomContextMenu);
  setScene(&scene_);
  scene_.setNetwork(network);
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
    if (auto c = qobject_cast<ConnectorGraphicsObjectPrivate *>(
            item->toGraphicsObject())) {
      if (!makingConnection_) {
        makingConnection_ = true;
        if (auto start =
                qobject_cast<OutputConnectorGraphicsObjectPrivate *>(c)) {
          connectionStart_ = start;
        } else {
          connectionEnd_ =
              qobject_cast<InputConnectorGraphicsObjectPrivate *>(c);
        }
        connectionLine_ =
            new QGraphicsLineItem{QLineF{c->scenePos(), c->scenePos()}};
        scene_.addItem(connectionLine_);
        resetConnection = false;
      } else

      {
        if (auto start =
                qobject_cast<OutputConnectorGraphicsObjectPrivate *>(c)) {
          if (connectionStart_ != nullptr) {
            // resetConnection = true;
          } else {
            connectionStart_ =
                qobject_cast<OutputConnectorGraphicsObjectPrivate *>(c);
          }
        } else {
          if (connectionEnd_ != nullptr) {
            // resetConnection = true;
          } else {
            connectionEnd_ =
                qobject_cast<InputConnectorGraphicsObjectPrivate *>(c);
          }
        }

        if (connectionStart_ && connectionEnd_) {
          // endpoint types must be different
          qDebug() << "making connection (" << connectionStart_ << " <-> "
                   << connectionEnd_ << ")";
          Q_EMIT connectionRequest(connectionStart_->output_,
                                   connectionEnd_->input_);
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
