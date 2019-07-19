#pragma once
#include "abstractnetworkmodel.h"
#include <QAbstractItemModel>
#include <QGraphicsLineItem>
#include <QGraphicsLinearLayout>
#include <QGraphicsObject>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QPersistentModelIndex>

class NetworkView;

//=============================================================================
class NodeConnectorGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  NodeConnectorGraphicsObjectPrivate(QPersistentModelIndex connectorIndex,
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
  QPersistentModelIndex connectorIndex_;
  bool hover_ = false;
};

//=============================================================================
class NodeGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  NodeGraphicsObjectPrivate(QSizeF size, QPersistentModelIndex nodeIndex,
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
  QPersistentModelIndex nodeIndex_;
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

  void setModel(AbstractNetworkModel *model);
  QAbstractItemModel *model() const;
  void createNodeVisual(int index);
  QList<QPersistentModelIndex> selectedNodes() const;

Q_SIGNALS:

private Q_SLOTS:
  void nodeAddedPrivate(int index);
  void nodeRemovedPrivate(int index);
  void nodeDeleteRequested(const QPersistentModelIndex &index);
  void inputConnectorAddedPrivate(const QModelIndex &parent, int index);
  void outputConnectorAddedPrivate(const QModelIndex &parent, int index);
  void inputConnectorRemovedPrivate(const QModelIndex &parent, int index);
  void outputConnectorRemovedPrivate(const QModelIndex &parent, int index);
  void connectionAddedPrivate(const QModelIndex &fromConnector,
                              const QModelIndex &toConnector);
  void connectionRemovedPrivate(const QModelIndex &fromConnector,
                                const QModelIndex &toConnector);

private:
  void updateConnections();
  AbstractNetworkModel *model_;
  std::vector<NodeGraphicsObjectPrivate *> nodes;
  std::vector<NodeConnectionGraphicsItemPrivate *> connections_;
};

//=============================================================================
class NetworkView : public QGraphicsView {
  Q_OBJECT
public:
  friend class NodeGraphicsObjectPrivate;
  friend class NodeConnectorGraphicsObjectPrivate;

  NetworkView(QWidget *parent = nullptr);
  ~NetworkView();

  void setModel(AbstractNetworkModel *model) { scene_.setModel(model); }

  QAbstractItemModel *model() const { return scene_.model(); }

  QList<QPersistentModelIndex> selectedNodes() const {
    return scene_.selectedNodes();
  }

Q_SIGNALS:
  void connectionRequest(QPersistentModelIndex fromConnector,
                         QPersistentModelIndex toConnector);

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