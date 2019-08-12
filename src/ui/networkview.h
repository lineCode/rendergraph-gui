#pragma once
#include "abstractnetworkmodel.h"
#include "render/node.h"
#include <QAbstractItemModel>
#include <QGraphicsLineItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsObject>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QPersistentModelIndex>
#include <unordered_map>

class NetworkView;

using render::Node;

//=============================================================================
class NodeConnectorGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  NodeConnectorGraphicsObjectPrivate(int connectorIndex,
                                     AbstractNetworkModel::ConnectionType type,
                                     QGraphicsItem *parent = nullptr);
  virtual ~NodeConnectorGraphicsObjectPrivate() {}
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

protected:
  AbstractNetworkModel::ConnectionType type_;
  int connectorIndex_;
  bool hover_ = false;
};

//=============================================================================
class NodeGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  NodeGraphicsObjectPrivate(QSizeF size, const Node *node,
                            QGraphicsItem *parent = nullptr);
  virtual ~NodeGraphicsObjectPrivate() {}

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

Q_SIGNALS:
  void deleteRequested(const QPersistentModelIndex &index);

private Q_SLOTS:
  void positionChanged();

private:
  QSizeF size_;
  bool hover_ = false;
  const Node *node_;
  render::Observer::Ptr nodeObserver_;
  QGraphicsWidget *inputConnectorsWidget_;
  QGraphicsWidget *outputConnectorsWidget_;
  QList<NodeConnectorGraphicsObjectPrivate *> inputConnectors_;
  QList<NodeConnectorGraphicsObjectPrivate *> outputConnectors_;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  void inputConnectorAdded(int index);
  void inputConnectorRemoved(int index);
  void outputConnectorAdded(int index);
  void outputConnectorRemoved(int index);

  void NodeGraphicsObjectPrivate::updateConnectorLayout(
      QList<NodeConnectorGraphicsObjectPrivate *> &connectors);
};

class NodeConnectionGraphicsItemPrivate : public QGraphicsPathItem {
public:
  friend class NetworkScene;
  NodeConnectionGraphicsItemPrivate(NodeConnectorGraphicsObjectPrivate *src,
                                    NodeConnectorGraphicsObjectPrivate *dst)
      : QGraphicsPathItem{}, srcConn_{src}, dstConn_{dst} {}

  void updatePositions();

private:
  NodeConnectorGraphicsObjectPrivate *srcConn_;
  NodeConnectorGraphicsObjectPrivate *dstConn_;
};

class NetworkScene : public QGraphicsScene {
  Q_OBJECT
public:
  friend class NodeGraphicsObjectPrivate;
  friend class NetworkView;
  NetworkScene(QObject *parent = nullptr);

  void setNetwork(Node *node);
  const Node *network() const;

  void createNodeVisual(const Node *node);

  QVector<const Node *> selectedNodes() const;

Q_SIGNALS:

private Q_SLOTS:
  void nodeAdded(const Node *node);
  void nodeRemoved(const Node *node);
  void inputConnectorAdded(const Node *node, int index);
  void outputConnectorAdded(const Node *node, int index);
  void inputConnectorRemoved(const Node *node, int index);
  void outputConnectorRemoved(const Node *node, int index);
  void connectionAdded(const Node *fromNode, const Node *toNode);
  void connectionRemoved(const Node *fromNode, const Node *toNode);

private:
  void updateConnections();
  Node *network_;
  render::Observer::Ptr networkObserver_;
  std::unordered_map<const Node *, NodeGraphicsObjectPrivate *> nodes_;
  std::vector<NodeConnectionGraphicsItemPrivate *> connections_;
};

//=============================================================================
class NetworkView : public QGraphicsView {
  Q_OBJECT
public:
  friend class NodeGraphicsObjectPrivate;
  friend class NodeConnectorGraphicsObjectPrivate;

  NetworkView(const Node *network = nullptr, QWidget *parent = nullptr);
  ~NetworkView();

  void setNetwork(render::Node *network) { scene_.setNetwork(network); }
  QVector<const Node *> selectedNodes() const { return scene_.selectedNodes(); }

Q_SIGNALS:
  void connectionRequest(const Node *fromNode, const Node *toNode);

public Q_SLOTS:
  void nodeAdded(const Node *node) { scene_.nodeAdded(node); }
  void nodeRemoved(const Node *node) { scene_.nodeRemoved(node); }
  void inputConnectorAdded(const Node *node, int index) {
    scene_.inputConnectorAdded(node, index);
  }
  void outputConnectorAdded(const Node *node, int index) {
    scene_.outputConnectorAdded(node, index);
  }
  void inputConnectorRemoved(const Node *node, int index) {
    scene_.inputConnectorRemoved(node, index);
  }
  void outputConnectorRemoved(const Node *node, int index) {
    scene_.outputConnectorRemoved(node, index);
  }
  void connectionAdded(const Node *fromNode, const Node *toNode) {
    scene_.connectionAdded(fromNode, toNode);
  }
  void connectionRemoved(const Node *fromNode, const Node *toNode) {
    scene_.connectionRemoved(fromNode, toNode);
  }

protected:
  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void wheelEvent(QWheelEvent *) override;
  void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
  void reset();

  QPoint mouseAnchor;
  QPoint translationBeforeDrag;
  NetworkScene scene_;
  bool makingConnection_ = false;
  NodeConnectorGraphicsObjectPrivate *connectionStart_ = nullptr;
  QGraphicsLineItem *connectionLine_ = nullptr;
};
