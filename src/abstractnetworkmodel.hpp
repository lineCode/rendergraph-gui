#pragma once
#include <QAbstractItemModel>

class AbstractNetworkModel : public QAbstractItemModel {
  Q_OBJECT
public:
  struct NodeID {
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
  };

  struct NodeIndex {
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

    ConnectorIndex outputConnectorByID(ConnectorID connector) const {
      networkModel()->outputConnectorByID(*this, connector);
    }

    ConnectorIndex outputConnection(int index) const {
      networkModel()->outputConnection(*this, index);
    }
  };

  struct ConnectionIndex {
    QModelIndex index;

    const AbstractNetworkModel *networkModel() const {
      return static_cast<const AbstractNetworkModel *>(index.model());
    }

    Endpoints endpoints() const { networkModel()->endpoints(*this); }
  };

  enum class ConnectionType { Input, Output };

  // adding a connector SHOULD NOT invalidate existing connections
  // adding a connection SHOULD NOT invalidate existing connections

  AbstractNetworkModel(QObject *parent = nullptr);
  ~AbstractNetworkModel();

  virtual int nodeCount() const = 0;
  virtual NodeIndex node(int index) const = 0;
  virtual NodeIndex nodeByID(NodeID id) const = 0;

  // Input connectors
  virtual int inputConnectorCount(const NodeIndex &parent) const = 0;
  virtual ConnectorIndex inputConnector(const NodeIndex &parent,
                                        int index) const = 0;
  virtual ConnectorIndex inputConnectorByID(const NodeIndex &parent,
                                            ConnectorID connector) const = 0;

  // Output connectors
  virtual int outputConnectorCount(const NodeIndex &parent) const = 0;
  virtual ConnectorIndex outputConnector(const NodeIndex &parent,
                                         int index) const = 0;
  virtual ConnectorIndex outputConnectorByID(const NodeIndex &parent,
                                             ConnectorID connector) const = 0;

  // Accessing connections
  // - return ConnectionIndex
  // - return Connection*
  // - return QList<ConnectionIndex>

  // Input connections
  virtual int inputConnectionCount(const NodeIndex &parent) const = 0;
  virtual ConnectionIndex inputConnection(const NodeIndex &parent,
                                          int index) const = 0;

  // Output connections
  virtual int outputConnectionCount(const NodeIndex &parent) const = 0;
  virtual ConnectionIndex outputConnection(const NodeIndex &parent,
                                           int index) const = 0;

  // Connection endpoints
  virtual Endpoints endpoints(const ConnectionIndex &connection) const = 0;

  // add/remove
  virtual bool addNode(NodeID id) { return false; }
  virtual bool removeNode(int index) { return false; }
  virtual bool removeNodeByID(NodeID id) { return false; }

  virtual bool insertInputConnector(const NodeIndex &parent, int indexBefore,
                                    ConnectorID id) {
    return false;
  }

  virtual bool removeInputConnector(const NodeIndex &parent, int index) {
    return false;
  }

  virtual bool removeInputConnectorByID(const NodeIndex &parent,
                                        ConnectorID id) {
    return false;
  }

  virtual bool insertOutputConnector(const NodeIndex &parent, int indexBefore,
                                     ConnectorID id) {
    return false;
  }

  virtual bool removeOutputConnector(const NodeIndex &parent, int index) {
    return false;
  }

  virtual bool removeOutputConnectorByID(const NodeIndex &parent,
                                         ConnectorID id) {
    return false;
  }

  virtual bool addConnection(NodeID from, ConnectorID fromConnector, NodeID to,
                             ConnectorID toConnector) {
    return false;
  }

Q_SIGNALS:
  void nodeAdded(int index);
  void nodeRemoved(int index, NodeID id);

  void connectorAdded(const NodeIndex &parent, ConnectionType type, int index);
  void connectorRemoved(const NodeIndex &parent, ConnectionType type,
                        int index);

  void connectionAdded(const NodeIndex &parent, ConnectionType type, int index);
  void connectionRemoved(const NodeIndex &parent, ConnectionType type,
                         int index);

protected:
};