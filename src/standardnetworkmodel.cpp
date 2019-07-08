#include "standardnetworkmodel.hpp"

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
  return QModelIndex();
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

bool StandardNetworkModel::checkNodeIndex(
    const AbstractNetworkModel::NodeIndex &index) const {
  if (index.index.parent().isValid()) {
    return false;
  }
  int row = index.index.row();
  if (row < 0 || row >= nodes_.size()) {
    return false;
  }
  return true;
}

AbstractNetworkModel::NodeIndex StandardNetworkModel::node(int index) const {
  return NodeIndex{createIndex(index, 0)};
}

AbstractNetworkModel::NodeIndex
StandardNetworkModel::nodeByID(NodeID id) const {
  auto index = findNode(id);
  if (index == -1)
    return NodeIndex{};
  return NodeIndex{createIndex(index, 0)};
}

int StandardNetworkModel::inputConnectorCount(
    const AbstractNetworkModel::NodeIndex &parent) const {
  if (!checkNodeIndex(parent)) {
    return 0;
  }
  return nodes_[parent.index.row()]->inputConnectors.size();
}

int StandardNetworkModel::outputConnectorCount(
    const AbstractNetworkModel::NodeIndex &parent) const {
  if (!checkNodeIndex(parent)) {
    return 0;
  }
  return nodes_[parent.index.row()]->outputConnectors.size();
}

AbstractNetworkModel::ConnectorIndex StandardNetworkModel::inputConnector(
    const AbstractNetworkModel::NodeIndex &parent, int index) const {
  if (!checkNodeIndex(parent)) {
    return {};
  }
  auto nc = inputConnectorCount(parent);
  if (index < 0 || index >= nc) {
    return {};
  }
  return ConnectorIndex{
      createIndex(index, 0, nodes_[parent.index.row()].get())};
}

AbstractNetworkModel::ConnectorIndex StandardNetworkModel::outputConnector(
    const AbstractNetworkModel::NodeIndex &parent, int index) const {
  if (!checkNodeIndex(parent)) {
    return {};
  }
  auto nc = inputConnectorCount(parent);
  if (index < 0 || index >= nc) {
    return {};
  }
  return ConnectorIndex{
      createIndex(index, 0, nodes_[parent.index.row()].get())};
}

AbstractNetworkModel::ConnectorIndex StandardNetworkModel::inputConnectorByID(
    const AbstractNetworkModel::NodeIndex &parent,
    ConnectorID connector) const {
  return ConnectorIndex();
}

AbstractNetworkModel::ConnectorIndex StandardNetworkModel::outputConnectorByID(
    const AbstractNetworkModel::NodeIndex &parent,
    ConnectorID connector) const {
  return ConnectorIndex();
}

int StandardNetworkModel::inputConnectionCount(
    const AbstractNetworkModel::NodeIndex &parent) const {
  return 0;
}

AbstractNetworkModel::ConnectionIndex StandardNetworkModel::inputConnection(
    const AbstractNetworkModel::NodeIndex &parent, int index) const {
  return ConnectionIndex();
}

int StandardNetworkModel::outputConnectionCount(
    const AbstractNetworkModel::NodeIndex &parent) const {
  return 0;
}

AbstractNetworkModel::ConnectionIndex StandardNetworkModel::outputConnection(
    const AbstractNetworkModel::NodeIndex &parent, int index) const {
  return ConnectionIndex();
}

AbstractNetworkModel::Endpoints StandardNetworkModel::endpoints(
    const AbstractNetworkModel::ConnectionIndex &connection) const {
  return Endpoints();
}

bool StandardNetworkModel::addNode(NodeID id) {
  if (findNode(id) != -1) {
    return false;
  }

  auto n = std::make_unique<Node>();
  n->id = id;
  insertNode(std::move(n));
  return true;
}

bool StandardNetworkModel::removeNode(int index) {
  if (index < 0 || index > nodes_.size()) {
    return false;
  }

  // remove all connections involving the node
  auto n = nodes_[index].get();
  auto id = n->id;
  for (auto &&inputConnection : n->inputConnections) {
    auto from = nodes_[findNode(inputConnection.nodeID)].get();
    from->outputConnections.erase(
        std::remove_if(from->outputConnections.begin(),
                       from->outputConnections.end(),
                       [=](const Connection &to) { return to.nodeID == id; }),
        from->outputConnections.end());
  }

  for (auto &&outputConnection : n->outputConnections) {
    auto to = nodes_[findNode(outputConnection.nodeID)].get();
    to->inputConnections.erase(
        std::remove_if(to->inputConnections.begin(), to->inputConnections.end(),
                                              [=](const Connection &from) {
                                                return from.nodeID == id;
                                              }),
        to->inputConnections.end());
  }

  // erase the node

  beginRemoveRows({}, index, index);
  nodes_.erase(nodes_.begin() + index);
  endRemoveRows();
  Q_EMIT nodeRemoved(index, id);
  return true;
}

bool StandardNetworkModel::removeNodeByID(NodeID id) { return false; }

bool StandardNetworkModel::insertInputConnector(const NodeIndex &parent,
                                                int indexAfter,
                                                ConnectorID id) {
  if (!checkNodeIndex(parent)) {
    return false;
  }
  auto n = nodes_[parent.index.row()].get();
  n->inputConnectors.insert(n->inputConnectors.begin() + indexAfter, id);
}

bool StandardNetworkModel::removeInputConnector(const NodeIndex &parent,
                                                int index) {
  return false;
}

bool StandardNetworkModel::removeInputConnectorByID(const NodeIndex &parent,
                                                    ConnectorID id) {
  return false;
}

bool StandardNetworkModel::insertOutputConnector(const NodeIndex &parent,
                                                 int indexAfter,
                                                 ConnectorID id) {
  return false;
}

bool StandardNetworkModel::removeOutputConnector(const NodeIndex &parent,
                                                 int index) {
  return false;
}

bool StandardNetworkModel::removeOutputConnectorByID(const NodeIndex &parent,
                                                     ConnectorID id) {
  return false;
}

bool StandardNetworkModel::addConnection(NodeID from, ConnectorID fromConnector,
                                         NodeID to, ConnectorID toConnector) {
  return false;
}

void StandardNetworkModel::insertNode(std::unique_ptr<Node> node) {
  // insert into sorted vector
  auto it = std::lower_bound(
      nodes_.begin(), nodes_.end(), node->id,
      [](const std::unique_ptr<Node> &n, NodeID id) { return n->id < id; });

  int row = 0;
  if (it == nodes_.end()) {
    row = nodes_.size();
  } else {
    row = it - nodes_.begin();
  }

  beginInsertRows({}, row, row);
  nodes_.insert(it, std::move(node));
  endInsertRows();
  Q_EMIT nodeAdded(row);
}

int StandardNetworkModel::findNode(NodeID id) const {
  auto it = std::lower_bound(
      nodes_.begin(), nodes_.end(), id,
      [](const std::unique_ptr<Node> &n, NodeID id) { return n->id < id; });

  if (it == nodes_.end()) {
    return -1;
  }
  return it - nodes_.begin();
}
