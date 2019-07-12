#include "standardnetworkmodel.hpp"

static const int INPUT_CONNECTOR_COLUMN = 1;
static const int OUTPUT_CONNECTOR_COLUMN = 2;
static const int CONNECTION_COLUMN = 3;

QModelIndex StandardNetworkModel::index(int row, int column,
	const QModelIndex &parent) const 
{
	if (!parent.isValid()) {
		// TODO check columns
		return createIndex(row, column, nodes_[row].get());
	}

	auto p = static_cast<QObject*>(parent.internalPointer());
	if (auto ptr = qobject_cast<NodePrivate *>(p)) {
		if (parent.column() == INPUT_CONNECTOR_COLUMN) {
			// TODO Check column == 0
			return createIndex(row, column, ptr->inputConnectors[row].get());
		}
		else if (parent.column() == OUTPUT_CONNECTOR_COLUMN) {
			// TODO Check column == 0
			return createIndex(row, column, ptr->outputConnectors[row].get());
		}
		else {
			return {};
		}
	}

	return {};
}

QModelIndex StandardNetworkModel::parent(const QModelIndex &child) const {
	auto obj = static_cast<QObject*>(child.internalPointer());
	if (auto ptr = qobject_cast<NodePrivate *>(obj)) {
		// pointer to parent node
		return {};
	}
	else if (auto ptr = qobject_cast<ConnectorPrivate *>(obj)) {
		// pointer to parent connector
		return ptr->parentIndex;
	}
	else {
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
	return createIndex(index, 0, nodes_[index].get());
}

int StandardNetworkModel::inputConnectorCount(const QModelIndex &nodeIndex) const {
	return derefNode(nodeIndex)->inputConnectors.size();
}

int StandardNetworkModel::outputConnectorCount(
	const QModelIndex &nodeIndex) const {
	return derefNode(nodeIndex)->outputConnectors.size();
}

QModelIndex StandardNetworkModel::inputConnector(const QModelIndex &nodeIndex,
	int index) const {
	auto c = derefNode(nodeIndex)->inputConnectors[index].get();
	return createIndex(index, 0, c);
}

QModelIndex StandardNetworkModel::outputConnector(const QModelIndex &nodeIndex,
	int index) const {
	auto c = derefNode(nodeIndex)->outputConnectors[index].get();
	return createIndex(index, 0, c);
}

// CONNECTIONS
int StandardNetworkModel::connectionCount(const QModelIndex &connectorIndex) const {

	return derefConnector(connectorIndex)->connectedTo.size();
}

QModelIndex StandardNetworkModel::connection(const QModelIndex &connectorIndex,
	int index) const
{
	return derefConnector(connectorIndex)->connectedTo[index];
}

// NODE INSERTION/REMOVAL
bool StandardNetworkModel::insertNode(int index) {
	auto n = std::make_unique<NodePrivate>();
	beginInsertRows({}, index, index);
	nodes_.insert(nodes_.begin() + index, std::move(n));
	endInsertRows();
	Q_EMIT nodeAdded(index);
	return true;
}

void StandardNetworkModel::removeConnectionsFromToNode(int index) {
	auto n = nodes_[index].get();
	auto ni = nodeIndex(index);

	for (int i = 0; i < n->inputConnectors.size(); ++i) {
		removeConnectionsOnConnector(inputConnector(ni, i));
	}
	for (int i = 0; i < n->outputConnectors.size(); ++i) {
		removeConnectionsOnConnector(outputConnector(ni, i));
	}
}

void StandardNetworkModel::removeConnectionsOnConnector(
	const QModelIndex &connectorIndex) {
	auto c = qobject_cast<ConnectorPrivate*>(static_cast<QObject*>(connectorIndex.internalPointer()));
	// remove all connections involving the connector
	for (int i = c->connectedTo.size() - 1; i >= 0; --i) {
		if (c->parentIndex.column() == OUTPUT_CONNECTOR_COLUMN) {
			removeConnection(connectorIndex, c->connectedTo[i]);
		}
		else {
			removeConnection(c->connectedTo[i], connectorIndex);
		}
	}
}

bool StandardNetworkModel::removeNode(int index) {
	if (index < 0 || index > nodes_.size()) {
		return false;
	}
	auto key = nodes_[index]->key;

	// remove all connections involving the node
	removeConnectionsFromToNode(index);

	// erase the node
	beginRemoveRows({}, index, index);
	nodes_.erase(nodes_.begin() + index);
	endRemoveRows();
	nodesByKey_.remove(key);

	Q_EMIT nodeRemoved(index);
	return true;
}

// INSERT/REMOVE CONNECTORS
bool StandardNetworkModel::insertInputConnector(const QModelIndex &nodeIndex,
	int index)
{
	auto n = derefNode(nodeIndex);
	auto c = std::make_unique<ConnectorPrivate>();
	c->parentIndex = this->index(nodeIndex.row(), INPUT_CONNECTOR_COLUMN);
	beginInsertRows(c->parentIndex, index, index);
	n->inputConnectors.insert(n->inputConnectors.begin() + index, std::move(c));
	endInsertRows();
	Q_EMIT inputConnectorAdded(nodeIndex, index);
	return true;
}

bool StandardNetworkModel::insertOutputConnector(const QModelIndex &nodeIndex,
	int index)
{
	auto n = derefNode(nodeIndex);
	auto c = std::make_unique<ConnectorPrivate>();
	c->parentIndex = this->index(nodeIndex.row(), OUTPUT_CONNECTOR_COLUMN);
	beginInsertRows(c->parentIndex, index, index);
	n->outputConnectors.insert(n->outputConnectors.begin() + index, std::move(c));
	endInsertRows();
	Q_EMIT outputConnectorAdded(nodeIndex, index);
	return true;
}

// Removing a connector also removes all connections attached to it
bool StandardNetworkModel::removeInputConnector(const QModelIndex &nodeIndex,
	int index) {
	auto n = derefNode(nodeIndex);
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
	auto n = derefNode(nodeIndex);
	removeConnectionsOnConnector(outputConnector(nodeIndex, index));
	beginRemoveRows(createIndex(nodeIndex.row(), OUTPUT_CONNECTOR_COLUMN), index,
		index);
	n->outputConnectors.erase(n->outputConnectors.begin() + index);
	endRemoveRows();
	Q_EMIT outputConnectorRemoved(nodeIndex, index);
	return true;
}

bool StandardNetworkModel::addConnection(const QModelIndex &fromConnector,
	const QModelIndex &toConnector)
{
	auto srcConn = derefConnector(fromConnector);
	auto dstConn = derefConnector(toConnector);

	if (srcConn->connectedTo.contains(toConnector)) {
		qDebug("WARNING: duplicate connection");
		return false;
	}

	{
		int index = srcConn->connectedTo.size();
		beginInsertRows(fromConnector, index, index);
		srcConn->connectedTo.push_back(toConnector);
		endInsertRows();
	}

	{
		int index = dstConn->connectedTo.size();
		beginInsertRows(toConnector, index, index);
		dstConn->connectedTo.push_back(fromConnector);
		endInsertRows();
	}


	Q_EMIT connectionAdded(fromConnector, toConnector);
	return true;
}

/*
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
}*/

bool StandardNetworkModel::removeConnection(const QModelIndex &fromConnector,
	const QModelIndex &toConnector) {
	//auto srcN = derefNode( static_cast<Node *>(fromConnector.internalPointer());
	//auto dstN = static_cast<Node *>(toConnector.internalPointer());

	auto srcConn = derefConnector(fromConnector);
	auto dstConn = derefConnector(toConnector);

	auto si = srcConn->connectedTo.indexOf(toConnector);
	auto di = dstConn->connectedTo.indexOf(fromConnector);
	if (si == -1 || di == -1) {
		qDebug("WARNING: could not find corresponding connection");
		return false;
	}

	beginRemoveRows(fromConnector,
		si, si);
	srcConn->connectedTo.removeAt(si);
	endRemoveRows();

	beginRemoveRows(toConnector,
		di, di);
	dstConn->connectedTo.removeAt(di);
	endRemoveRows();

	Q_EMIT connectionRemoved(fromConnector, toConnector);
	return true;
}
