#pragma once
#include "abstractnetworkmodel.hpp"
#include <QAbstractItemModel>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsView>

class NodeGraphicsObject : public QGraphicsObject {
  Q_OBJECT
public:
  NodeGraphicsObject(QSizeF size, QGraphicsItem *parent = nullptr);

  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

protected:
  QSizeF size_;
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

class NetworkView : public QGraphicsView {
  Q_OBJECT
public:
  NetworkView(QWidget *parent = nullptr);
  ~NetworkView();

  void setModel(AbstractNetworkModel *model);
  QAbstractItemModel *model() const;

private Q_SLOTS:
  void nodeAddedPrivate(int index);
  void nodeRemovedPrivate(int index);
  void connectorAddedPrivate(const AbstractNetworkModel::NodeIndex &parent,
                             AbstractNetworkModel::ConnectionType type,
                             int index);
  void connectorRemovedPrivate(const AbstractNetworkModel::NodeIndex &parent,
                               AbstractNetworkModel::ConnectionType type,
                               int index);
  void connectionAddedPrivate(const AbstractNetworkModel::NodeIndex &parent,
                              AbstractNetworkModel::ConnectionType type,
                              int index);
  void connectionRemovedPrivate(const AbstractNetworkModel::NodeIndex &parent,
                                AbstractNetworkModel::ConnectionType type,
                                int index);

protected:
  // void paintEvent(QPaintEvent*) override;
  // void resizeEvent(QResizeEvent*) override;
  void mousePressEvent(QMouseEvent *) override;
  void mouseMoveEvent(QMouseEvent *) override;
  void wheelEvent(QWheelEvent *) override;
  // void scrollContentsBy(int dx, int dy) override;
  // void drawWidget(QPainter&, QPaintEvent*);
  void contextMenuEvent(QContextMenuEvent *e) override;

  void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
  void reset();
  void createNodeVisual(int index);
  void removeNodeVisual(int index);

  QPoint mouseAnchor;
  QPoint translationBeforeDrag;
  AbstractNetworkModel *model_;
  QGraphicsScene scene_;
  QGraphicsItemGroup *gridGroup_;
  std::vector<QGraphicsItem *> nodes;
};