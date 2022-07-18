//
// Created by sayan on 07.07.2022.
//

#include "CacheModel.h"
#include "Cache.h"
#include <queue>

namespace Mikran {

CacheModel::CacheModel(Cache *t_cache, QObject *parent)
	: QAbstractItemModel(parent)
	, m_cache(t_cache)
{
}

QVariant CacheModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		std::string &data = m_cache->getData(index.internalPointer()).name;
		return data.c_str();
	}

	return QVariant();
}

Qt::ItemFlags CacheModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (m_cache->getData(index.internalPointer()).deleted)
		return QAbstractItemModel::flags(index) & ~Qt::ItemIsEnabled;

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

bool CacheModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	CacheNode &data = m_cache->getData(index.internalPointer());
	std::string new_value = std::move(value.toString().toStdString());

	if (new_value.empty())
		return false;

	data.name = std::move(value.toString().toStdString());

	if (data.state != State::CREATED)
		data.state = State::CHANGED;

	emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

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

	Cache::TreeNodeDescriptor parent_node;

	if (!parent.isValid()) {
		parent_node = m_cache->getRoot();
	} else {
		parent_node = parent.internalPointer();
	}
	int counter = 0;

	for (auto node : boost::make_iterator_range(m_cache->getChildrenIterator(parent_node))) {
		if (counter == row) {
			return createIndex(row, column, node);
		}
		counter++;
	}
	return QModelIndex();
}

QModelIndex CacheModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	auto parent = m_cache->getParent(index.internalPointer());

	if (m_cache->getRoot() == parent)
		return QModelIndex();

	return createIndex(m_cache->getData(parent).row, 0, parent);
}

int CacheModel::rowCount(const QModelIndex &parent) const
{
	Cache::TreeNodeDescriptor node;

	if (parent.isValid()) {
		node = parent.internalPointer();
	} else {
		node = m_cache->getRoot();
	}
	return m_cache->getData(node).count;
}

int CacheModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

void CacheModel::loadIndex(const QModelIndex &index)
{
	Cache::TreeNodeDescriptor parent_node;
	Cache::TreeNodeDescriptor new_node;

	parent_node = m_cache->findParent(index.internalPointer());

	if (parent_node == nullptr)
		return;

	if (parent_node == m_cache->getRoot()) {
		int first = m_cache->getData(parent_node).count;

		beginInsertRows(QModelIndex(), first, first);
		new_node = m_cache->fetchFromDatabase(parent_node, index.internalPointer());
		endInsertRows();
	} else {
		CacheNode &parent_node_data = m_cache->getData(parent_node);

		beginInsertRows(createIndex(parent_node_data.row, 0, parent_node), parent_node_data.count,
				parent_node_data.count);
		new_node = m_cache->fetchFromDatabase(parent_node, index.internalPointer());
		endInsertRows();
	}

	auto root_child_iterator = m_cache->getChildrenIterator(m_cache->getRoot());
	int sourceFirst = 0, destinationChild = 0;

	for (auto node = root_child_iterator.first; node != root_child_iterator.second;) {
		if (m_cache->isParent(new_node, *node)) {
			beginMoveRows(QModelIndex(), sourceFirst, sourceFirst,
					createIndex(m_cache->getData(new_node).row, 0, new_node), destinationChild);
			auto move_node = *node;
			node++;
			m_cache->move(new_node, move_node);
			m_cache->getData(m_cache->getRoot()).count--;
			m_cache->getData(move_node).row = m_cache->getData(new_node).count++;

			for (auto root_child = node; root_child != root_child_iterator.second; root_child++) {
				m_cache->getData(*root_child).row--;
			}
			endMoveRows();
			destinationChild++;
		} else {
			node++;
			sourceFirst++;
		}
	}

	if (!m_cache->getData(parent_node).deleted)
		return;

	std::queue<Cache::TreeNodeDescriptor> bfs_traversal;
	m_cache->getData(new_node).deleted = true;
	bfs_traversal.push(new_node);

	while (!bfs_traversal.empty()) {
		for (auto child : boost::make_iterator_range(m_cache->getChildrenIterator(bfs_traversal.front()))) {
			CacheNode &data = m_cache->getData(child);

			if (data.deleted)
				continue;

			data.deleted = true;

			bfs_traversal.push(child);
		}
		bfs_traversal.pop();
	}
}

void CacheModel::deleteIndex(const QModelIndex &index)
{
	Cache::TreeNodeDescriptor node = index.internalPointer();
	CacheNode &node_data = m_cache->getData(node);

	node_data.deleted = true;
	if (node_data.state != State::CREATED) {
		node_data.state = State::DELETED;
	} else {
		node_data.state = State::NOT_CHANGED;
	}
	emit dataChanged(index, index, {Qt::DisplayRole});

	std::queue<Cache::TreeNodeDescriptor> bfs_traversal;
	bfs_traversal.push(node);

	while (!bfs_traversal.empty()) {
		for (auto child : boost::make_iterator_range(m_cache->getChildrenIterator(bfs_traversal.front()))) {
			CacheNode &data = m_cache->getData(child);

			if (data.deleted)
				continue;

			data.deleted = true;
			QModelIndex deleted_index = createIndex(data.row, 0, child);
			emit dataChanged(deleted_index, deleted_index, {Qt::DisplayRole});

			bfs_traversal.push(child);
		}
		bfs_traversal.pop();
	}
}

void CacheModel::addIndex(const QModelIndex &index, QString name)
{
	Cache::TreeNodeDescriptor parent = index.internalPointer();
	CacheNode &parent_data = m_cache->getData(parent);

	beginInsertRows(index, parent_data.count, parent_data.count);
	Cache::TreeNodeDescriptor node = m_cache->createChild(parent);
	CacheNode &data = m_cache->getData(node);

	data.name = std::move(name.toStdString());
	data.row = parent_data.count;
	data.state = State::CREATED;

	parent_data.count++;

	endInsertRows();
}

void CacheModel::applyChanges()
{
	m_cache->flush();
}

void CacheModel::resetModel()
{
	beginResetModel();
	m_cache->reset();
	endResetModel();
}

}
