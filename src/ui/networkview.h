#pragma once
#include "abstractnetworkmodel.h"
#include "node/node.h"
#include "node/observer.h"
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
	enum class Kind {
		Input,
		Output
	};
  friend class NetworkScene;
  friend class NetworkView;
  ConnectorGraphicsObjectPrivate(node::Node *node, Kind kind,
                                 QGraphicsItem *parent = nullptr);

  virtual ~ConnectorGraphicsObjectPrivate();
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

  virtual std::string tooltip() const = 0;
  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  Kind kind() const { return kind_; }

protected:
	Kind kind_;
  node::Node *node_;
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
  InputConnectorGraphicsObjectPrivate(node::Node *node, node::Input *input,
                                      QGraphicsItem *parent = nullptr)
      : ConnectorGraphicsObjectPrivate{node, ConnectorGraphicsObjectPrivate::Kind::Input, parent}, input_{input} {}

  std::string tooltip() const override;
  //void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  //void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

protected:
  node::Input *input_;
};

//=============================================================================
class OutputConnectorGraphicsObjectPrivate
    : public ConnectorGraphicsObjectPrivate {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  friend class NodeGraphicsObjectPrivate;
  OutputConnectorGraphicsObjectPrivate(node::Node *node,
                                       node::Output *output,
                                       QGraphicsItem *parent = nullptr)
      : ConnectorGraphicsObjectPrivate{node, ConnectorGraphicsObjectPrivate::Kind::Output, parent}, output_{output} {}

  std::string tooltip() const override;
  //void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  //void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;

protected:
  node::Output *output_;
};

//=============================================================================
class NodeGraphicsObjectPrivate : public QGraphicsObject {
  Q_OBJECT
public:
  friend class NetworkScene;
  friend class NetworkView;
  NodeGraphicsObjectPrivate(QSizeF size, node::Node *node,
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
  InputConnectorGraphicsObjectPrivate *findInputConnector(node::Input *input);
  OutputConnectorGraphicsObjectPrivate *
  findOutputConnector(node::Output *output);
  void inputConnectorAdded(node::Input *input);
  void inputConnectorRemoved(node::Input *input);
  void outputConnectorAdded(node::Output *output);
  void outputConnectorRemoved(node::Output *output);

  void hoverEnterEvent(QGraphicsSceneHoverEvent *) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *) override;
  void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
  void dropEvent(QGraphicsSceneDragDropEvent *event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  void NodeGraphicsObjectPrivate::updateConnectorLayout(
      QList<ConnectorGraphicsObjectPrivate *> &connectors);

  QSizeF size_;
  bool hover_ = false;
  node::Node *node_;
  node::Observer::Ptr nodeObserver_;
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

  void setNetwork(node::Node *node);
  node::Node *network() const;

  void createNodeVisual(node::Node *node);

  QVector<node::Node *> selectedNodes() const;

Q_SIGNALS:

private Q_SLOTS:
  void nodeAdded(node::Node *node);
  void nodeRemoved(node::Node *node);
  void inputConnectorAdded(node::Node *node, node::Input *input);
  void outputConnectorAdded(node::Node *node, node::Output *output);
  void inputConnectorRemoved(node::Node *node, node::Input *input);
  void outputConnectorRemoved(node::Node *node, node::Output *output);
  void connectionAdded(node::Node *source, node::Output *output,
                       node::Node *destination, node::Input *input);
  void connectionRemoved(node::Node *source, node::Output *output,
                         node::Node *destination, node::Input *input);

private:
  void updateConnections();
  node::Node *network_;
  node::Observer::Ptr networkObserver_;
  std::unordered_map<node::Node *, NodeGraphicsObjectPrivate *> nodes_;
  std::vector<NodeConnectionGraphicsItemPrivate *> connections_;
};

//=============================================================================
class NetworkView : public QGraphicsView {
  Q_OBJECT
public:
  friend class NodeGraphicsObjectPrivate;

  NetworkView(node::Node *network = nullptr, QWidget *parent = nullptr);
  ~NetworkView();

  void setNetwork(node::Node *network) { scene_.setNetwork(network); }
  QVector<node::Node *> selectedNodes() const {
    return scene_.selectedNodes();
  }

Q_SIGNALS:
  void connectionRequest(node::Node *from, node::Output *output,
                         node::Node *to, node::Input *input);

public Q_SLOTS:
  void nodeAdded(node::Node *node) { scene_.nodeAdded(node); }
  void nodeRemoved(node::Node *node) { scene_.nodeRemoved(node); }
  void inputConnectorAdded(node::Node *node, node::Input *input) {
    scene_.inputConnectorAdded(node, input);
  }
  void outputConnectorAdded(node::Node *node, node::Output *output) {
    scene_.outputConnectorAdded(node, output);
  }
  void inputConnectorRemoved(node::Node *node, node::Input *input) {
    scene_.inputConnectorRemoved(node, input);
  }
  void outputConnectorRemoved(node::Node *node, node::Output *output) {
    scene_.outputConnectorRemoved(node, output);
  }
  void connectionAdded(node::Node *source, node::Output *output,
                       node::Node *destination, node::Input *input) {
    scene_.connectionAdded(source, output, destination, input);
  }
  void connectionRemoved(node::Node *source, node::Output *output,
                         node::Node *destination, node::Input *input) {
    scene_.connectionRemoved(source, output, destination, input);
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
