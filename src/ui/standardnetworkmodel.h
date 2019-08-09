#pragma once
#include "abstractnetworkmodel.h"
#include <QSet>
#include <algorithm>
#include <memory>
#include <unordered_map>

class ConnectorPrivate : public QObject {
  Q_OBJECT
public:
  quint64 key = 0; // unique in each connector group
  QPersistentModelIndex parentIndex;
  QList<QPersistentModelIndex> connectedTo;
};

class NodePrivate : public QObject {
  Q_OBJECT
public:
  quint64 key; // unique in each node
  std::vector<std::unique_ptr<ConnectorPrivate>> inputConnectors;
  std::vector<std::unique_ptr<ConnectorPrivate>> outputConnectors;

  int findInputConnectorByKey(quint64 key) const {
    auto it = std::find_if(inputConnectors.begin(), inputConnectors.end(),
                           [key](const std::unique_ptr<ConnectorPrivate> &c) {
                             return c->key == key;
                           });
    if (it == inputConnectors.end())
      return -1;
    return it - inputConnectors.begin();
  }

  int findOutputConnectorByKey(quint64 key) const {
    auto it = std::find_if(outputConnectors.begin(), outputConnectors.end(),
                           [key](const std::unique_ptr<ConnectorPrivate> &c) {
                             return c->key == key;
                           });
    if (it == outputConnectors.end())
      return -1;
    return it - outputConnectors.begin();
  }
};

// Synchronization:
// on node/connector added, send message
// OR: record commands
//
// How to identify nodes across the application?
// IDs, then map IDs->PersistentModelIndex

class StandardNetworkModel : public AbstractNetworkModel {
  Q_OBJECT
public:
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &child) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  // via AbstractNetworkModel
  int nodeCount() const override;
  QModelIndex nodeIndex(int index) const override;
  int inputConnectorCount(const QModelIndex &nodeIndex) const override;
  QModelIndex inputConnector(const QModelIndex &nodeIndex,
                             int index) const override;
  int outputConnectorCount(const QModelIndex &nodeIndex) const override;
  QModelIndex outputConnector(const QModelIndex &nodeIndex,
                              int index) const override;
  int connectionCount(const QModelIndex &connectorIndex) const override;
  QModelIndex connection(const QModelIndex &connectorIndex,
                         int index) const override;

  bool insertNode(int index) override;
  bool removeNode(int index) override;
  bool insertInputConnector(const QModelIndex &nodeIndex, int index) override;
  bool removeInputConnector(const QModelIndex &nodeIndex, int index) override;
  bool insertOutputConnector(const QModelIndex &nodeIndex, int index) override;
  bool removeOutputConnector(const QModelIndex &nodeIndex, int index) override;
  bool addConnection(const QModelIndex &fromConnector,
                     const QModelIndex &toConnector) override;
  bool removeConnection(const QModelIndex &fromConnector,
                        const QModelIndex &toConnector) override;

  // Key API
  quint64 nodeKey(const QModelIndex &nodeIndex) const {
    return derefNode(nodeIndex)->key;
  }

  quint64 connectorKey(const QModelIndex &connectorIndex) const {
    return derefConnector(connectorIndex)->key;
  }

  bool insertNodeWithKey(quint64 key) {
    insertNode(0);
    nodes_[0]->key = key;
    nodesByKey_.insert(key, nodeIndex(0));
    return true;
  }

  QModelIndex nodeByKey(const QModelIndex &parent, quint64 key) const {
    if (nodesByKey_.contains(key)) {
      return nodesByKey_[key];
    }
    return {};
  }

  QModelIndex inputConnectorByKey(const QModelIndex &nodeIndex,
                                  quint64 key) const {
    auto i = derefNode(nodeIndex)->findInputConnectorByKey(key);
    if (i == -1) {
      return {};
    }
    return inputConnector(nodeIndex, i);
  }

  QModelIndex outputConnectorByKey(const QModelIndex &nodeIndex,
                                   quint64 key) const {
    auto i = derefNode(nodeIndex)->findOutputConnectorByKey(key);
    if (i == -1) {
      return {};
    }
    return outputConnector(nodeIndex, i);
  }

  bool insertInputConnectorWithKey(const QModelIndex &parent, int index,
                                   quint64 key) {
    insertInputConnector(parent, index);
    derefConnector(inputConnector(parent, index))->key = key;
  }

  bool insertOutputConnectorWithKey(const QModelIndex &parent, int index,
                                    quint64 key) {
    insertOutputConnector(parent, index);
    derefConnector(outputConnector(parent, index))->key = key;
  }

private:
  NodePrivate *derefNode(const QModelIndex &nodeIndex) {
    return qobject_cast<NodePrivate *>(
        static_cast<QObject *>(nodeIndex.internalPointer()));
  }

  const NodePrivate *derefNode(const QModelIndex &nodeIndex) const {
    return qobject_cast<const NodePrivate *>(
        static_cast<const QObject *>(nodeIndex.internalPointer()));
  }

  ConnectorPrivate *derefConnector(const QModelIndex &connectorIndex) {
    return qobject_cast<ConnectorPrivate *>(
        static_cast<QObject *>(connectorIndex.internalPointer()));
  }
  const ConnectorPrivate *
  derefConnector(const QModelIndex &connectorIndex) const {
    return qobject_cast<const ConnectorPrivate *>(
        static_cast<const QObject *>(connectorIndex.internalPointer()));
  }

  bool checkNodeIndex(const QModelIndex &index) const;
  void removeConnectionsFromToNode(int index);
  void removeConnectionsOnConnector(const QModelIndex &connector);

  std::vector<std::unique_ptr<NodePrivate>> nodes_;
  QHash<quint64, QPersistentModelIndex> nodesByKey_;
};
