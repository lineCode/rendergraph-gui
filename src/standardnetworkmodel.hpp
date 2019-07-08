#pragma once
#include "abstractnetworkmodel.hpp"
#include <memory>
#include <unordered_map>

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
  NodeIndex node(int index) const override;
  NodeIndex nodeByID(NodeID id) const override;
  int inputConnectorCount(const NodeIndex &parent) const override;
  ConnectorIndex inputConnector(const NodeIndex &parent,
                                int index) const override;
  ConnectorIndex inputConnectorByID(const NodeIndex &parent,
                                    ConnectorID connector) const override;
  int outputConnectorCount(const NodeIndex &parent) const override;
  ConnectorIndex outputConnector(const NodeIndex &parent,
                                 int index) const override;
  ConnectorIndex outputConnectorByID(const NodeIndex &parent,
                                     ConnectorID connector) const override;
  int inputConnectionCount(const NodeIndex &parent) const override;
  ConnectionIndex inputConnection(const NodeIndex &parent,
                                  int index) const override;
  int outputConnectionCount(const NodeIndex &parent) const override;
  ConnectionIndex outputConnection(const NodeIndex &parent,
                                   int index) const override;
  Endpoints endpoints(const ConnectionIndex &connection) const override;

  bool addNode(NodeID id) override;
  bool removeNode(int index) override;
  bool removeNodeByID(NodeID id) override;
  bool insertInputConnector(const NodeIndex &parent, int indexAfter,
                            ConnectorID id) override;
  bool removeInputConnector(const NodeIndex &parent, int index) override;
  bool removeInputConnectorByID(const NodeIndex &parent,
                                ConnectorID id) override;
  bool insertOutputConnector(const NodeIndex &parent, int indexAfter,
                             ConnectorID id) override;
  bool removeOutputConnector(const NodeIndex &parent, int index) override;
  bool removeOutputConnectorByID(const NodeIndex &parent,
                                 ConnectorID id) override;
  bool addConnection(NodeID from, ConnectorID fromConnector, NodeID to,
                     ConnectorID toConnector) override;

private:
  struct Connection {
    AbstractNetworkModel::NodeID nodeID;
    AbstractNetworkModel::ConnectorID connectorID;
  };

  struct Node {
    NodeID id;
    std::vector<AbstractNetworkModel::ConnectorID> inputConnectors;
    std::vector<AbstractNetworkModel::ConnectorID> outputConnectors;
    std::vector<Connection> inputConnections;
    std::vector<Connection> outputConnections;
  };

  bool checkNodeIndex(const NodeIndex &index) const;
  int findNode(NodeID id) const;
  void insertNode(std::unique_ptr<Node> node);

  // Sorted by node ID
  std::vector<std::unique_ptr<Node>> nodes_;
};