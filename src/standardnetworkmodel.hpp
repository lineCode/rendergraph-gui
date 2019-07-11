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
  QModelIndex nodeIndex(int index) const override;
  int inputConnectorCount(const QModelIndex &parent) const override;
  QModelIndex inputConnector(const QModelIndex &parent,
                                int index) const override;
  int outputConnectorCount(const QModelIndex &parent) const override;
  QModelIndex outputConnector(const QModelIndex &parent,
                                 int index) const override;
  int connectionCount(const QModelIndex &parent) const override;
  QModelIndex connection(const QModelIndex &parent,
                                  int index) const override;
  //Endpoints endpoints(const QModelIndex &connection) const override;

  bool insertNode(int index) override;
  bool removeNode(int index) override;
  bool insertInputConnector(const QModelIndex &parent, int index) override;
  bool removeInputConnector(const QModelIndex &parent, int index) override;
  bool insertOutputConnector(const QModelIndex &parent, int index) override;
  bool removeOutputConnector(const QModelIndex &parent, int index) override;
  bool addConnection(const QModelIndex& fromConnector, const QModelIndex& toConnector) override;
  bool removeConnection(const QModelIndex &fromConnector,
                        const QModelIndex &toConnector) override;

private:
  struct Connection {
    ConnectionType type;
    QPersistentModelIndex thisConnector;
    QPersistentModelIndex otherConnector;
  };
  struct Node {
	QPersistentModelIndex thisIndex;
    std::vector<QVariant> inputConnectors;
    std::vector<QVariant> outputConnectors;
    std::vector<Connection> connections;
  };

  int findConnection(const Node* n,
                     const QModelIndex &fromConnector,
                     const QModelIndex &toConnector,
                     StandardNetworkModel::ConnectionType connType) const;
  bool checkNodeIndex(const QModelIndex &index) const;
  void removeConnectionsFromToNode(int index);
  void removeConnectionsOnConnector(const QModelIndex &connector);


  std::vector<std::unique_ptr<Node>> nodes_;
};