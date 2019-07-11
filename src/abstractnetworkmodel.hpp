#pragma once
#include <QAbstractItemModel>

/*
class AbstractNetworkModel2
{
	Q_OBJECT
public:
	struct NodeIndex {
		quint64 key = 0;

		bool isValid() const { return key != 0; }
	};

	struct ConnectorIndex {
		NodeIndex node;
		quint64 key;
		
		bool isValid() const { return key != 0; }
	};

	virtual int nodeCount() const = 0;
	virtual QList<NodeIndex> nodes() const = 0;

	virtual int inputConnectorCount(const NodeIndex& node) const = 0;
	virtual QList<ConnectorIndex> inputConnectors(const NodeIndex& node) const = 0;
	virtual int outputConnectorCount(const NodeIndex& node) const = 0;
	virtual QList<ConnectorIndex> outputConnectors(const NodeIndex& node) const = 0;

	virtual int connectionCount(const ConnectorIndex &connector) const = 0;
	virtual QList<ConnectorIndex> connections(const ConnectorIndex &connector) const = 0;

	virtual NodeIndex addNode() { return {}; }
	virtual bool removeNode(const NodeIndex& node) { return false; }

	virtual ConnectorIndex insertInputConnector(const NodeIndex& node, int index) { return {}; }
	virtual bool removeInputConnector(const ConnectorIndex&) { return false; }
	virtual ConnectorIndex insertOutputConnector(const NodeIndex& node, int index) { return {}; }
	virtual bool removeOutputConnector(const ConnectorIndex&) { return false; }

	virtual bool addConnection(const ConnectorIndex& fromConnector, const ConnectorIndex& toConnector) { return false; }
	virtual bool removeConnection(const ConnectorIndex &fromConnector, const ConnectorIndex &toConnector) { return false; }

Q_SIGNALS:
	void nodeAdded(NodeIndex index);
	void nodeRemoved(NodeIndex index);

	void inputConnectorAdded(const ConnectorIndex&);
	void inputConnectorRemoved(const ConnectorIndex&);
	void outputConnectorAdded(const ConnectorIndex&);
	void outputConnectorRemoved(const ConnectorIndex&);

	void connectionAdded(const ConnectorIndex &fromConnector, const ConnectorIndex &toConnector);
	void connectionRemoved(const ConnectorIndex &fromConnector, const ConnectorIndex &toConnector);
};*/

class AbstractNetworkModel : public QAbstractItemModel {
  Q_OBJECT
public:
   enum class ConnectionType { Input, Output };

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

  // Returns the number of connections on a connector.
  virtual int connectionCount(const QModelIndex &connectorIndex) const = 0;
  virtual QModelIndex connection(const QModelIndex &connectorIndex, int index) const = 0;

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