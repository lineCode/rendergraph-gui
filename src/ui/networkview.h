#pragma once
#include "abstractnetworkmodel.h"
#include "render/node.h"
#include "render/observer.h"
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

class ConnectorGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  ConnectorGraphicsObjectPrivate(QGraphicsItem *parent = nullptr);
  virtual ~ConnectorGraphicsObjectPrivate() {}
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

protected:
  bool hover_ = false;
};

//=============================================================================

class InputConnectorGraphicsObjectPrivate
    : public ConnectorGraphicsObjectPrivate {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  friend class NodeGraphicsObjectPrivate;
  InputConnectorGraphicsObjectPrivate(render::Input *input,
                                      QGraphicsItem *parent = nullptr)
      : ConnectorGraphicsObjectPrivate{parent}, input_{input} {}

protected:
  render::Input *input_;
  bool hover_ = false;
};

//=============================================================================
class OutputConnectorGraphicsObjectPrivate
    : public ConnectorGraphicsObjectPrivate {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  friend class NodeGraphicsObjectPrivate;
  OutputConnectorGraphicsObjectPrivate(render::Output *output,
                                       QGraphicsItem *parent = nullptr)
      : ConnectorGraphicsObjectPrivate{parent}, output_{output} {}

protected:
  render::Output *output_;
  bool hover_ = false;
};

//=============================================================================
class NodeGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  NodeGraphicsObjectPrivate(QSizeF size, render::Node *node,
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
  InputConnectorGraphicsObjectPrivate *findInputConnector(render::Input *input) {
    for (auto c : inputConnectors_) {
      auto ci = static_cast<InputConnectorGraphicsObjectPrivate *>(c);
      if (ci->input_ == input) {
        return ci;
      }
    }
    return nullptr;
  }

  OutputConnectorGraphicsObjectPrivate *findOutputConnector(render::Output *output) {
    for (auto c : outputConnectors_) {
      auto ci = static_cast<OutputConnectorGraphicsObjectPrivate *>(c);
      if (ci->output_ == output) {
        return ci;
      }
    }
    return nullptr;
  }

  QSizeF size_;
  bool hover_ = false;
  render::Node *node_;
  render::Observer::Ptr nodeObserver_;
  QGraphicsWidget *inputConnectorsWidget_;
  QGraphicsWidget *outputConnectorsWidget_;
  QList<ConnectorGraphicsObjectPrivate *> inputConnectors_;
  QList<ConnectorGraphicsObjectPrivate *> outputConnectors_;

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
      QList<ConnectorGraphicsObjectPrivate *> &connectors);
};

class NodeConnectionGraphicsItemPrivate : public QGraphicsPathItem {
public:
  friend class NetworkScene;
  NodeConnectionGraphicsItemPrivate(OutputConnectorGraphicsObjectPrivate *src,
                                    InputConnectorGraphicsObjectPrivate *dst)
      : QGraphicsPathItem{}, srcConn_{src}, dstConn_{dst} {}

  void updatePositions();

private:
  OutputConnectorGraphicsObjectPrivate *srcConn_;
  InputConnectorGraphicsObjectPrivate *dstConn_;
};

class NetworkScene : public QGraphicsScene {
  Q_OBJECT
public:
  friend class NodeGraphicsObjectPrivate;
  friend class NetworkView;
  NetworkScene(QObject *parent = nullptr);

  void setNetwork(render::Node *node);
  render::Node *network() const;

  void createNodeVisual(render::Node *node);

  QVector<render::Node *> selectedNodes() const;

Q_SIGNALS:

private Q_SLOTS:
  void nodeAdded(render::Node *node);
  void nodeRemoved(render::Node *node);
  void inputConnectorAdded(render::Node *node, int index);
  void outputConnectorAdded(render::Node *node, int index);
  void inputConnectorRemoved(render::Node *node, int index);
  void outputConnectorRemoved(render::Node *node, int index);
  void connectionAdded(render::Output *from, render::Input *to);
  void connectionRemoved(render::Output *from, render::Input *to);

private:
  void updateConnections();
  render::Node *network_;
  render::Observer::Ptr networkObserver_;
  std::unordered_map<render::Node *, NodeGraphicsObjectPrivate *> nodes_;
  std::vector<NodeConnectionGraphicsItemPrivate *> connections_;
};

//=============================================================================
class NetworkView : public QGraphicsView {
  Q_OBJECT
public:
  friend class NodeGraphicsObjectPrivate;

  NetworkView(render::Node *network = nullptr, QWidget *parent = nullptr);
  ~NetworkView();

  void setNetwork(render::Node *network) { scene_.setNetwork(network); }
  QVector<render::Node *> selectedNodes() const {
    return scene_.selectedNodes();
  }

Q_SIGNALS:
  void connectionRequest(render::Output *from, render::Input *to);

public Q_SLOTS:
  void nodeAdded(render::Node *node) { scene_.nodeAdded(node); }
  void nodeRemoved(render::Node *node) { scene_.nodeRemoved(node); }
  void inputConnectorAdded(render::Node *node, int index) {
    scene_.inputConnectorAdded(node, index);
  }
  void outputConnectorAdded(render::Node *node, int index) {
    scene_.outputConnectorAdded(node, index);
  }
  void inputConnectorRemoved(render::Node *node, int index) {
    scene_.inputConnectorRemoved(node, index);
  }
  void outputConnectorRemoved(render::Node *node, int index) {
    scene_.outputConnectorRemoved(node, index);
  }
  void connectionAdded(render::Output *from, render::Input *to) {
    scene_.connectionAdded(from, to);
  }
  void connectionRemoved(render::Output *from, render::Input *to) {
    scene_.connectionRemoved(from, to);
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
  OutputConnectorGraphicsObjectPrivate *connectionStart_ = nullptr;
  InputConnectorGraphicsObjectPrivate *connectionEnd_ = nullptr;
  QGraphicsLineItem *connectionLine_ = nullptr;
};
