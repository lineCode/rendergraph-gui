#pragma once
#include "abstractnetworkmodel.hpp"
#include <QAbstractItemModel>
#include <QGraphicsLinearLayout>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QPersistentModelIndex>
#include <QGraphicsLineItem>
#include <QGraphicsPathItem>

class NetworkView;


//=============================================================================
class NodeConnectorGraphicsObject : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkView;
  NodeConnectorGraphicsObject(QPersistentModelIndex connectorIndex,
                              AbstractNetworkModel::ConnectionType type,
                              QGraphicsItem *parent = nullptr);
  virtual ~NodeConnectorGraphicsObject() {}
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  //void mousePressEvent(QGraphicsSceneMouseEvent *) override;
  //void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;

protected:
  AbstractNetworkModel::ConnectionType type_;
  QPersistentModelIndex connectorIndex_;
  bool hover_ = false;
};

//=============================================================================
class NodeGraphicsObject : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkView;
  NodeGraphicsObject(QSizeF size, QPersistentModelIndex nodeIndex,
	  QGraphicsItem *parent = nullptr);
  virtual ~NodeGraphicsObject() {}

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

Q_SIGNALS:
  void deleteRequested(const QPersistentModelIndex &index);

private:
  QSizeF size_;
  bool hover_ = false;
  QPersistentModelIndex nodeIndex_;
  QGraphicsWidget *inputConnectorsWidget_;
  QGraphicsWidget *outputConnectorsWidget_;
  QList<NodeConnectorGraphicsObject *> inputConnectors_;
  QList<NodeConnectorGraphicsObject *> outputConnectors_;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  void inputConnectorAdded(int index);
  void inputConnectorRemoved(int index);
  void outputConnectorAdded(int index);
  void outputConnectorRemoved(int index);

  void NodeGraphicsObject::updateConnectorLayout(
      QList<NodeConnectorGraphicsObject *> &connectors);
};

class NetworkScene : public QGraphicsScene {
  Q_OBJECT
public:
  NetworkScene(QObject *parent = nullptr)
      : QGraphicsScene{parent}
	  {}

Q_SIGNALS:
};

//=============================================================================
class NetworkView : public QGraphicsView {
  Q_OBJECT
public:
  friend class NodeGraphicsObject;
  friend class NodeConnectorGraphicsObject;

  NetworkView(QWidget *parent = nullptr);
  ~NetworkView();

  void setModel(AbstractNetworkModel *model);
  QAbstractItemModel *model() const;

  QList<QPersistentModelIndex> selectedNodes() const;

Q_SIGNALS:
  void connectionRequest(QPersistentModelIndex fromConnector, QPersistentModelIndex toConnector);

private Q_SLOTS:
  void nodeAddedPrivate(int index);
  void nodeRemovedPrivate(int index);
  void nodeDeleteRequested(const QPersistentModelIndex &index);
  void inputConnectorAddedPrivate(const QModelIndex &parent, int index);
  void outputConnectorAddedPrivate(const QModelIndex &parent, int index);
  void inputConnectorRemovedPrivate(const QModelIndex &parent, int index);
  void outputConnectorRemovedPrivate(const QModelIndex &parent, int index);
  void connectionAddedPrivate(const QModelIndex& fromConnector, const QModelIndex& toConnector);

protected:
  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void wheelEvent(QWheelEvent *) override;
  void drawBackground(QPainter *painter, const QRectF &rect) override;


private:
  void reset();
  void createNodeVisual(int index);

  QPoint mouseAnchor;
  QPoint translationBeforeDrag;
  AbstractNetworkModel *model_;
  NetworkScene scene_;
  QGraphicsItemGroup *gridGroup_;
  std::vector<NodeGraphicsObject *> nodes;
  std::vector<QGraphicsPathItem *> connections_;
  bool makingConnection_ = false;
  QPersistentModelIndex connectionStart_;
  AbstractNetworkModel::ConnectionType connectionStartType_;
  QGraphicsLineItem *connectionLine_ = nullptr;
};