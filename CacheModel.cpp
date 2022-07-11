//
// Created by sayan on 07.07.2022.
//

#include "CacheModel.h"
#include "Cache.h"
#include "DatabaseModel.h"
#include <queue>

CacheModel::CacheModel(Cache &cache, QObject *parent) : QAbstractItemModel(parent), cache(cache)
{
}

QVariant CacheModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		std::string &data = cache.getNodeData(index.internalPointer()).name;
		return data.c_str();
	}

	return QVariant();
}

Qt::ItemFlags CacheModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Cache::tree_node_descriptor current = index.internalPointer();

	while (current != cache.getRootNode()) {
		if (cache.getNodeData(current).deleted)
			return QAbstractItemModel::flags(index) & ~Qt::ItemIsEnabled;
		current = cache.getParentNode(current);
	}
	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

bool CacheModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	cache_node_data &data = cache.getNodeData(index.internalPointer());
	std::string new_value = std::move(value.toString().toStdString());

	if (new_value.empty())
		return false;

	data.name = std::move(value.toString().toStdString());
	emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

	if (data.database_node != nullptr)
		data.modified = true;

	return true;
}

QVariant CacheModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return "Cache";

	return QVariant();
}

QModelIndex CacheModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	Cache::tree_node_descriptor parent_node;

	if (!parent.isValid()) {
		parent_node = cache.getRootNode();
	} else {
		parent_node = parent.internalPointer();
	}
	auto iterator = cache.getNodeChildrenIterator(parent_node);
	auto node = iterator.first;

	for (int i = 0; i < row && node != iterator.second; i++)
		node++;
	if (node == iterator.second)
		return QModelIndex();

	return createIndex(row, column, *node);
}

QModelIndex CacheModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto parent = cache.getParentNode(index.internalPointer());

	if (cache.getRootNode() == parent)
		return QModelIndex();

	return createIndex(cache.getNodeData(parent).row, 0, parent);
}

int CacheModel::rowCount(const QModelIndex &parent) const
{
	Cache::tree_node_descriptor node;

	if (parent.isValid()) {
		node = parent.internalPointer();
	} else {
		node = cache.getRootNode();
	}

	return cache.getNodeData(node).count;
}

int CacheModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

void CacheModel::loadNode(const QModelIndex &index)
{
	Database::tree_node_descriptor database_node = reinterpret_cast<Database::tree_node_descriptor>(index.internalPointer());
	Cache::tree_node_descriptor parent_node;
	Cache::tree_node_descriptor new_node;

	parent_node = cache.findParent(database_node);

	if (parent_node == nullptr)
		return;

	if (parent_node == cache.getRootNode()) {
		int first = cache.getNodeData(parent_node).count;

		beginInsertRows(QModelIndex(), first, first);
		new_node = cache.fetchFromDatabase(parent_node,
				reinterpret_cast<Database::tree_node_descriptor>(index.internalPointer()));
		endInsertRows();
	} else {
		cache_node_data &parent_node_data = cache.getNodeData(parent_node);

		beginInsertRows(createIndex(parent_node_data.row, 0, parent_node), parent_node_data.count, parent_node_data.count);
		new_node = cache.fetchFromDatabase(parent_node,
				reinterpret_cast<Database::tree_node_descriptor>(index.internalPointer()));
		endInsertRows();
	}

	auto root_child_iterator = cache.getNodeChildrenIterator(cache.getRootNode());
	int sourceFirst = 0, destinationChild = 0;

	for (auto node = root_child_iterator.first; node != root_child_iterator.second;) {
		if (cache.isParent(new_node, *node)) {
			beginMoveRows(QModelIndex(), sourceFirst, sourceFirst,
					createIndex(cache.getNodeData(new_node).row, 0, new_node), destinationChild);
			auto move_node = *node++;
			cache.moveNode(new_node, move_node);
			for (auto root_child = node; root_child != root_child_iterator.second; root_child++) {
				cache.getNodeData(*root_child).row--;
			}
			endMoveRows();
			destinationChild++;
		} else {
			node++;
			sourceFirst++;
		}
	}
}

void CacheModel::deleteNode(const QModelIndex &index)
{
	Cache::tree_node_descriptor node = index.internalPointer();
	cache.getNodeData(node).deleted = true;
	emit dataChanged(index, index, {Qt::DisplayRole});

//	beginRemoveRows(index.parent(), index.row(), index.row());
//	cache.getNodeData(cache.getParentNode(node)).count--;
//	cache.removeNode(node);
//	endRemoveRows();
}

void CacheModel::addNode(const QModelIndex &index, QString name)
{
	Cache::tree_node_descriptor parent = index.internalPointer();
	int first = cache.getNodeData(parent).count;

	beginInsertRows(index, first, first);
	cache.getNodeData(parent).count++;
	Cache::tree_node_descriptor node = cache.createChildNode(parent);
	cache_node_data &data = cache.getNodeData(node);
	data.name = std::move(name.toStdString());
	data.row = first;
	endInsertRows();
}

void CacheModel::applyChanges(QAbstractItemModel *model)
{
	DatabaseModel *databaseModel = reinterpret_cast<DatabaseModel *>(model);
	std::queue<Cache::tree_node_descriptor> bfs_queue;
	bfs_queue.push(cache.getRootNode());

	while (!bfs_queue.empty()) {
		Cache::tree_node_descriptor current = bfs_queue.front();

		for (auto child : boost::make_iterator_range(cache.getNodeChildrenIterator(current))) {
			cache_node_data &data = cache.getNodeData(child);
			if (data.database_node == nullptr) {
				Cache::tree_node_descriptor parent = cache.getParentNode(child);
				data.database_node = databaseModel->createNode(cache.getNodeData(parent).database_node, data);
			} else if (data.deleted) {
				databaseModel->deleteNode(data.database_node);
			} else if (data.modified) {
				databaseModel->changeNodeData(data);
				data.modified = false;
			}
			bfs_queue.push(child);
		}
		bfs_queue.pop();
	}
}

void CacheModel::resetModel()
{
	beginResetModel();
	cache.reset();
	endResetModel();
}
