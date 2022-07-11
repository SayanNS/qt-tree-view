//
// Created by sayan on 05.07.2022.
//

#include "DatabaseModel.h"
#include "Database.h"
#include "Cache.h"
#include <queue>


DatabaseModel::DatabaseModel(Database &database, QObject *parent)
	: QAbstractItemModel(parent), database(database)
{
}

QVariant DatabaseModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	std::string data = database.getNodeData(
			index.internalPointer()).name;

	return data.c_str();
}

Qt::ItemFlags DatabaseModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Database::tree_node_descriptor current = index.internalPointer();

	while (current != database.getRootNode()) {
		if (database.getNodeData(current).deleted)
			return QAbstractItemModel::flags(index) & ~Qt::ItemIsEnabled;
		current = database.getParentNode(current);
	}
	if (database.getNodeData(current).deleted)
		return QAbstractItemModel::flags(index) & ~Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}

QVariant DatabaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return "Database";

	return QVariant();
}

QModelIndex DatabaseModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (!parent.isValid())
		return createIndex(row, column, database.getRootNode());

	auto iterator = database.getNodeChildrenIterator(parent.internalPointer());
	auto node = iterator.first;

	for (int i = 0; i < row && node != iterator.second; i++)
		node++;
	if (node == iterator.second)
		return QModelIndex();

	return createIndex(row, column, *node);
}

QModelIndex DatabaseModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	if (database.getRootNode() == index.internalPointer())
		return QModelIndex();

	Database::tree_node_descriptor parent = database.getParentNode(index.internalPointer());

	if (parent == database.getRootNode()) {
		return createIndex(0, 0, database.getRootNode());
	}

	return createIndex(database.getNodeData(parent).row, 0, parent);
}

int DatabaseModel::rowCount(const QModelIndex &parent) const
{
	Database::tree_node_descriptor node;

	if (parent.isValid()) {
		node = parent.internalPointer();
		return database.getNodeData(node).count;
	} else {
		if (database.getRootNode() == nullptr)
			return 0;
		return 1;
	}
}

int DatabaseModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

Database::tree_node_descriptor DatabaseModel::createNode(Database::tree_node_descriptor parent,
		const cache_node_data &cacheNodeData)
{
	database_node_data &parentNodeData = database.getNodeData(parent);

	beginInsertRows(createIndex(parentNodeData.row, 0, parent), parentNodeData.count, parentNodeData.count);
	Database::tree_node_descriptor newNode = database.createChildNode(parent);
	database_node_data &newNodeData = database.getNodeData(newNode);
	newNodeData.row = parentNodeData.count++;
	newNodeData.name = cacheNodeData.name;
	endInsertRows();

	return newNode;
}

void DatabaseModel::deleteNode(Database::tree_node_descriptor node)
{
	database_node_data &data = database.getNodeData(node);

	if (data.deleted)
		return;

	data.deleted = true;
	QModelIndex index = createIndex(data.row, 0, node);
	emit dataChanged(index, index, {Qt::DisplayRole});
}

void DatabaseModel::changeNodeData(const cache_node_data &cacheNodeData)
{
	database_node_data &data = database.getNodeData(cacheNodeData.database_node);
	data.name = cacheNodeData.name;
	QModelIndex index = createIndex(data.row, 0, cacheNodeData.database_node);
	emit dataChanged(index, index, {Qt::DisplayRole});
}

void DatabaseModel::resetModel()
{
	beginResetModel();
	database.reset();
	endResetModel();
}
