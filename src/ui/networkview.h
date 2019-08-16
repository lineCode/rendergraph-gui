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
  ConnectorGraphicsObjectPrivate(render::Node* node, QGraphicsItem *parent = nullptr);

  virtual ~ConnectorGraphicsObjectPrivate() {}
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

protected:
	render::Node* node_;
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
  InputConnectorGraphicsObjectPrivate(render::Node* node, render::Input* input,
                                      QGraphicsItem *parent = nullptr)
      : ConnectorGraphicsObjectPrivate{node, parent}, input_{ input } {}

protected:
  render::Input* input_;
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
  OutputConnectorGraphicsObjectPrivate(render::Node* node, render::Output* output,
                                       QGraphicsItem *parent = nullptr)
      : ConnectorGraphicsObjectPrivate{node, parent}, output_{ output } {}

protected:
	render::Output* output_;
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
	InputConnectorGraphicsObjectPrivate *findInputConnector(render::Input* input);
	OutputConnectorGraphicsObjectPrivate *findOutputConnector(render::Output* output);
  void inputConnectorAdded(render::Input* input);
  void inputConnectorRemoved(render::Input* input);
  void outputConnectorAdded(render::Output* output);
  void outputConnectorRemoved(render::Output* output);

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;


  void NodeGraphicsObjectPrivate::updateConnectorLayout(
	  QList<ConnectorGraphicsObjectPrivate *> &connectors);

  QSizeF size_;
  bool hover_ = false;
  render::Node *node_;
  render::Observer::Ptr nodeObserver_;
  QGraphicsWidget *inputConnectorsWidget_;
  QGraphicsWidget *outputConnectorsWidget_;
  QList<ConnectorGraphicsObjectPrivate *> inputConnectors_;
  QList<ConnectorGraphicsObjectPrivate *> outputConnectors_;

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
  void inputConnectorAdded(render::Node *node, render::Input* input);
  void outputConnectorAdded(render::Node *node, render::Output* output);
  void inputConnectorRemoved(render::Node *node, render::Input* input);
  void outputConnectorRemoved(render::Node *node, render::Output* output);
  void connectionAdded(render::Node* from, render::Output* output, render::Node *to, render::Input* input);
  void connectionRemoved(render::Node* from, render::Output* output, render::Node *to, render::Input* input);

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
  void connectionRequest(render::Node* from, render::Output* output, render::Node *to, render::Input* input);

public Q_SLOTS:
  void nodeAdded(render::Node *node) { scene_.nodeAdded(node); }
  void nodeRemoved(render::Node *node) { scene_.nodeRemoved(node); }
  void inputConnectorAdded(render::Node *node, render::Input* input) {
    scene_.inputConnectorAdded(node, input);
  }
  void outputConnectorAdded(render::Node *node, render::Output* output) {
    scene_.outputConnectorAdded(node, output);
  }
  void inputConnectorRemoved(render::Node *node, render::Input* input) {
    scene_.inputConnectorRemoved(node, input);
  }
  void outputConnectorRemoved(render::Node *node, render::Output* output) {
    scene_.outputConnectorRemoved(node, output);
  }
  void connectionAdded(render::Node* from, render::Output* output, render::Node *to, render::Input* input) {
    scene_.connectionAdded(from, output, to, input);
  }
  void connectionRemoved(render::Node* from, render::Output* output, render::Node *to, render::Input* input) {
    scene_.connectionRemoved(from, output, to, input);
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
