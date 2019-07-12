#include "abstractnetworkmodel.hpp"

AbstractNetworkModel::AbstractNetworkModel(QObject *parent)
	: QAbstractItemModel{ parent } {}

AbstractNetworkModel::~AbstractNetworkModel() {}