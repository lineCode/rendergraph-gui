#pragma once
#include <QAbstractItemModel>

class AbstractNetworkModel : public QAbstractItemModel {
  Q_OBJECT
public:
  /*struct NodeID {
    int id;
    bool operator==(const NodeID &other) const { return id == other.id; }
    bool operator<(const NodeID &other) const { return id < other.id; }
  };

  struct ConnectorID {
    int id;
    bool operator==(const ConnectorID &other) const { return id == other.id; }
    bool operator<(const ConnectorID &other) const { return id < other.id; }
  };

  struct Endpoints {
    NodeID from;
    ConnectorID fromConnector;
    NodeID to;
    ConnectorID toConnector;
  };

  struct ConnectorIndex {
    QModelIndex index;
  };*/

  /*struct NodeIndex {
    QModelIndex index;

    const AbstractNetworkModel *networkModel() const {
      return static_cast<const AbstractNetworkModel *>(index.model());
    }

    ConnectorIndex inputConnector(int index) const {
      networkModel()->inputConnector(*this, index);
    }

    ConnectorIndex inputConnectorByID(ConnectorID connector) const {
      networkModel()->inputConnectorByID(*this, connector);
    }

    ConnectorIndex inputConnection(int index) const {
      networkModel()->inputConnection(*this, index);
    }

    ConnectorIndex outputConnector(int index) const {
      networkModel()->outputConnector(*this, index);
    }

    ConnectorIndex outputConnection(int index) const {
      networkModel()->outputConnection(*this, index);
    }
  };*/

  /*struct ConnectionIndex {
    QModelIndex index;

    const AbstractNetworkModel *networkModel() const {
      return static_cast<const AbstractNetworkModel *>(index.model());
    }

    Endpoints endpoints() const { networkModel()->endpoints(*this); }
  };*/

  enum class ConnectionType { Input, Output };

  // adding a connector SHOULD NOT invalidate existing connections
  // adding a connection SHOULD NOT invalidate existing connections

  AbstractNetworkModel(QObject *parent = nullptr);
  ~AbstractNetworkModel();

  virtual int nodeCount() const = 0;
  virtual QModelIndex nodeIndex(int index) const = 0;

  // Input connectors
  virtual int inputConnectorCount(const QModelIndex &nodeIndex) const = 0;
  virtual QModelIndex inputConnector(const QModelIndex &nodeIndex,
                                        int index) const = 0;

  // Output connectors
  virtual int outputConnectorCount(const QModelIndex &nodeIndex) const = 0;
  virtual QModelIndex outputConnector(const QModelIndex &nodeIndex,
                                         int index) const = 0;

  // connections
  virtual int connectionCount(const QModelIndex &parent) const = 0;
  virtual QModelIndex connection(const QModelIndex &parent,
                                int index) const = 0;

  // Connection endpoints
  //virtual Endpoints endpoints(const QModelIndex &connection) const = 0;

  // add/remove
  virtual bool insertNode(int index) { return false; }
  virtual bool removeNode(int index) { return false; }

  virtual bool insertInputConnector(const QModelIndex &parent, int index) {
    return false;
  }

  virtual bool removeInputConnector(const QModelIndex &parent, int index) {
    return false;
  }

  virtual bool insertOutputConnector(const QModelIndex &parent, int index) {
    return false;
  }

  virtual bool removeOutputConnector(const QModelIndex &parent, int index) {
    return false;
  }

  virtual bool addConnection(const QModelIndex& fromConnector, const QModelIndex& toConnector) {
    return false;
  }

  virtual bool removeConnection(const QModelIndex &fromConnector,
                                const QModelIndex &toConnector) {
    return false;
  }


Q_SIGNALS:
  void nodeAdded(int index);
  void nodeRemoved(int index);

  void inputConnectorAdded(const QModelIndex &parentNode, int index);
  void inputConnectorRemoved(const QModelIndex &parentNode,
                             int index);
  void outputConnectorAdded(const QModelIndex &parentNode, int index);
  void outputConnectorRemoved(const QModelIndex &parentNode,
                             int index);

  void connectionAdded(const QModelIndex &fromConnector,
                       const QModelIndex &toConnector);
  void connectionRemoved(const QModelIndex &fromConnector,
                         const QModelIndex &toConnector);

protected:
};