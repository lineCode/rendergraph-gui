#include "standardnetworkmodel.hpp"

static const int INPUT_CONNECTOR_COLUMN = 1;
static const int OUTPUT_CONNECTOR_COLUMN = 2;
static const int CONNECTION_COLUMN = 3;

QModelIndex StandardNetworkModel::index(int row, int column,
                                        const QModelIndex &parent) const {
  if (parent.isValid()) {
    return {};
  }

  if (column < 0 || column > 1) {
    return {};
  }

  return createIndex(row, column);
}

QModelIndex StandardNetworkModel::parent(const QModelIndex &child) const {
  if (auto ptr = static_cast<Node *>(child.internalPointer())) {
    // pointer to parent node
    return ptr->thisIndex;
  } else {
    return {};
  }
}

int StandardNetworkModel::rowCount(const QModelIndex &parent) const {
  return nodeCount();
}

int StandardNetworkModel::columnCount(const QModelIndex &parent) const {
  return 1;
}

QVariant StandardNetworkModel::data(const QModelIndex &index, int role) const {
  // TODO
  return {};
}

int StandardNetworkModel::nodeCount() const { return nodes_.size(); }

bool StandardNetworkModel::checkNodeIndex(const QModelIndex &index) const {
  if (index.parent().isValid()) {
    return false;
  }
  int row = index.row();
  if (row < 0 || row >= nodes_.size()) {
    return false;
  }
  return true;
}

QModelIndex StandardNetworkModel::nodeIndex(int index) const {
  return createIndex(index, 0);
}

int StandardNetworkModel::inputConnectorCount(const QModelIndex &parent) const {
  if (!checkNodeIndex(parent)) {
    return 0;
  }
  return nodes_[parent.row()]->inputConnectors.size();
}

int StandardNetworkModel::outputConnectorCount(
    const QModelIndex &parent) const {
  if (!checkNodeIndex(parent)) {
    return 0;
  }
  return nodes_[parent.row()]->outputConnectors.size();
}

QModelIndex StandardNetworkModel::inputConnector(const QModelIndex &nodeIndex,
                                                 int index) const {
  if (!checkNodeIndex(nodeIndex)) {
    return {};
  }
  auto n = nodes_[nodeIndex.row()].get();
  if (index < 0 || index >= n->inputConnectors.size()) {
    return {};
  }

  return createIndex(index, 0, n);
}

QModelIndex StandardNetworkModel::outputConnector(const QModelIndex &nodeIndex,
                                                  int index) const {
  if (!checkNodeIndex(nodeIndex)) {
    return {};
  }
  auto n = nodes_[nodeIndex.row()].get();
  if (index < 0 || index >= n->outputConnectors.size()) {
    return {};
  }

  return createIndex(index, 0, n);
}

// CONNECTIONS
int StandardNetworkModel::connectionCount(const QModelIndex &nodeIndex) const {
  if (!checkNodeIndex(nodeIndex)) {
    return 0;
  }
  return nodes_[nodeIndex.row()]->connections.size();
}

QModelIndex StandardNetworkModel::connection(const QModelIndex &nodeIndex,
                                             int index) const {
  if (!checkNodeIndex(nodeIndex)) {
    return {};
  }
  auto n = nodes_[nodeIndex.row()].get();
  if (index < 0 || index >= n->connections.size()) {
    return {};
  }

  return createIndex(index, 0, n);
}

// NODE INSERTION/REMOVAL
bool StandardNetworkModel::insertNode(int index) {
  auto n = std::make_unique<Node>();
  beginInsertRows({}, index, index);
  nodes_.insert(nodes_.begin() + index, std::move(n));
  endInsertRows();
  Q_EMIT nodeAdded(index);
  return true;
}

void StandardNetworkModel::removeConnectionsFromToNode(int index) {
  auto n = nodes_[index].get();
  for (auto &&c : n->connections) {
    auto nn = nodes_[c.otherConnector.parent().row()].get();
    for (int i = nn->connections.size() - 1; i >= 0; ++i) {
      if (nn->connections[i].otherConnector.parent() == n->thisIndex) {
        if (nn->connections[i].type == ConnectionType::Input) {
          removeConnection(nn->connections[i].otherConnector,
                           nn->connections[i].thisConnector);
        } else {
          removeConnection(nn->connections[i].thisConnector,
                           nn->connections[i].otherConnector);
		}
      }
    }
  }
}

void StandardNetworkModel::removeConnectionsOnConnector(
    const QModelIndex &connector) {
  auto n = static_cast<Node *>(connector.internalPointer());
  // remove all connections involving the connector
  for (int i = n->connections.size() - 1; i >= 0; ++i) {
    if (n->connections[i].thisConnector == connector) {
      if (n->connections[i].type == ConnectionType::Input) {
        removeConnection(n->connections[i].otherConnector, connector);
	  }
    }
  }
}

bool StandardNetworkModel::removeNode(int index) {
  if (index < 0 || index > nodes_.size()) {
    return false;
  }

  // remove all connections involving the node
  removeConnectionsFromToNode(index);

  // erase the node
  beginRemoveRows({}, index, index);
  nodes_.erase(nodes_.begin() + index);
  endRemoveRows();
  Q_EMIT nodeRemoved(index);
  return true;
}

// INSERT/REMOVE CONNECTORS
bool StandardNetworkModel::insertInputConnector(const QModelIndex &nodeIndex,
                                                int index) {
  if (!checkNodeIndex(nodeIndex)) {
    return false;
  }
  auto n = nodes_[nodeIndex.row()].get();
  beginInsertRows(createIndex(nodeIndex.row(), INPUT_CONNECTOR_COLUMN), index,
                  index);
  n->inputConnectors.insert(n->inputConnectors.begin() + index, QVariant{});
  endInsertRows();
  Q_EMIT inputConnectorAdded(nodeIndex, index);
  return true;
}

bool StandardNetworkModel::insertOutputConnector(const QModelIndex &nodeIndex,
                                                 int index) {
  if (!checkNodeIndex(nodeIndex)) {
    return false;
  }
  auto n = nodes_[nodeIndex.row()].get();
  beginInsertRows(createIndex(nodeIndex.row(), OUTPUT_CONNECTOR_COLUMN), index,
                  index);
  n->outputConnectors.insert(n->outputConnectors.begin() + index, QVariant{});
  endInsertRows();
  Q_EMIT outputConnectorAdded(nodeIndex, index);
  return true;
}

// Removing a connector also removes all connections attached to it
bool StandardNetworkModel::removeInputConnector(const QModelIndex &nodeIndex,
                                                int index) {
  auto n = nodes_[index].get();
  removeConnectionsOnConnector(inputConnector(nodeIndex, index));
  beginRemoveRows(createIndex(nodeIndex.row(), INPUT_CONNECTOR_COLUMN), index,
                  index);
  n->inputConnectors.erase(n->inputConnectors.begin() + index);
  endRemoveRows();
  Q_EMIT inputConnectorRemoved(nodeIndex, index);
  return true;
}

bool StandardNetworkModel::removeOutputConnector(const QModelIndex &nodeIndex,
                                                 int index) {
  auto n = nodes_[index].get();
  removeConnectionsOnConnector(outputConnector(nodeIndex, index));
  beginRemoveRows(createIndex(nodeIndex.row(), INPUT_CONNECTOR_COLUMN), index,
                  index);
  n->inputConnectors.erase(n->inputConnectors.begin() + index);
  endRemoveRows();
  Q_EMIT outputConnectorRemoved(nodeIndex, index);
  return true;
}

bool StandardNetworkModel::addConnection(const QModelIndex &fromConnector,
                                         const QModelIndex &toConnector) {
  return false;
  /*auto nFrom = nodes_[fromConnector.parent().row()].get();
auto nTo = nodes_[toConnector.parent().row()].get();
//nFrom.

beginRemoveRows(createIndex(nodeIndex.row(), CONNECTION_COLUMN), connIndex,
            connIndex);
n->connections.erase(n->connections.begin() + connIndex);
endRemoveRows();*/
}

int StandardNetworkModel::findConnection(
    const Node *n, const QModelIndex &fromConnector,
    const QModelIndex &toConnector,
    StandardNetworkModel::ConnectionType connType) const {
  auto index = -1;
  for (int i = 0; i < n->connections.size(); ++i) {
    if (connType == ConnectionType::Input) {
      // look for incoming connection (from = other, to = this)
    }
    if (n->connections[i].otherConnector == fromConnector &&
        n->connections[i].thisConnector == toConnector) {
      index = i;
    }
  }
  return index;
}

bool StandardNetworkModel::removeConnection(const QModelIndex &fromConnector,
                                            const QModelIndex &toConnector) {
  auto srcN = static_cast<Node *>(fromConnector.internalPointer());
  auto dstN = static_cast<Node *>(fromConnector.internalPointer());
  auto srcConnIndex =
      findConnection(srcN, fromConnector, toConnector, ConnectionType::Output);
  auto dstConnIndex =
      findConnection(dstN, fromConnector, toConnector, ConnectionType::Input);

  if (srcConnIndex == -1 || dstConnIndex == -1) {
    qDebug("WARNING: could not find corresponding connection in remote node");
    return false;
  }

  beginRemoveRows(createIndex(fromConnector.parent().row(), CONNECTION_COLUMN),
                  srcConnIndex, srcConnIndex);
  srcN->connections.erase(srcN->connections.begin() + srcConnIndex);
  endRemoveRows();

  beginRemoveRows(createIndex(toConnector.parent().row(), CONNECTION_COLUMN),
                  srcConnIndex, srcConnIndex);
  dstN->connections.erase(dstN->connections.begin() + dstConnIndex);
  endRemoveRows();

  Q_EMIT connectionRemoved(fromConnector, toConnector);
  return true;
}
