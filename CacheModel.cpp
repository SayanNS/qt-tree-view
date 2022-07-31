//
// Created by sayan on 07.07.2022.
//

#include "CacheModel.h"
#include "Cache.h"


namespace Mikran {

CacheModel::CacheModel(Cache *cache, QObject *parent)
	: QAbstractItemModel(parent)
	, cache(cache)
{
	cache->setOnNewNodeAboutToBeInsertedHandler([this] (Cache::TreeNode *parent, int position)
	{
		if (parent == this->cache->getRoot()) {
			beginInsertRows(QModelIndex(), position, position);
		} else {
			beginInsertRows(createIndex(parent->getIndex(), 0, parent), position, position);
		}
	});

	cache->setOnNewNodeInsertedHandler([this] ()
	{
		endInsertRows();
	});

	cache->setOnNodeAboutToBeRemovedHandler([this] (Cache::TreeNode *parent, int position)
	{
		if (parent == this->cache->getRoot()) {
			beginRemoveRows(QModelIndex(), position, position);
		} else {
			beginRemoveRows(createIndex(parent->getIndex(), 0, parent), position, position);
		}
	});

	cache->setOnNodeRemovedHandler([this] ()
	{
		endRemoveRows();
	});

	cache->setOnNodeDataChangedHandler([this] (Cache::TreeNode *node)
	{
		QModelIndex index = createIndex(node->getIndex(), 0, node);
		emit dataChanged(index, index, {Qt::DisplayRole});
	});
}

QVariant CacheModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole) {
		return QVariant();
	}

	return static_cast<Cache::TreeNode *>(index.internalPointer())->name;
}

Qt::ItemFlags CacheModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (static_cast<Cache::TreeNode *>(index.internalPointer())->deleted)
		return QAbstractItemModel::flags(index) & ~Qt::ItemIsEnabled;

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

bool CacheModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	QString name = value.toString();

	if (name.isEmpty())
		return false;

	CacheNode *node = static_cast<Cache::TreeNode *>(index.internalPointer());
	node->name = name;
	node->state = NodeState::MODIFIED;
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

	Cache::TreeNode *parentNode;

	if (parent.isValid()) {
		parentNode = static_cast<Cache::TreeNode *>(parent.internalPointer());
	} else {
		parentNode = cache->getRoot();
	}

	return createIndex(row, column, parentNode->getChildrenAtIndex(row));
}

QModelIndex CacheModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	Cache::TreeNode *parent = static_cast<Cache::TreeNode *>(index.internalPointer())->getParent();

	if (cache->getRoot() == parent)
		return QModelIndex();

	return createIndex(parent->getIndex(), 0, parent);
}

int CacheModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<Cache::TreeNode *>(parent.internalPointer())->getChildrenCount();

	return cache->getRoot()->getChildrenCount();
}

int CacheModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

void CacheModel::loadIndex(const QModelIndex &index)
{
	cache->loadFromDatabase(static_cast<Database::TreeNode *>(index.internalPointer()));
}

void CacheModel::deleteIndex(const QModelIndex &index)
{
	cache->markNodeAndDescendantsAsDeleted(static_cast<Cache::TreeNode *>(index.internalPointer()));
}

void CacheModel::addIndex(const QModelIndex &index, QString &name)
{
	cache->createAndAppendNewNode(static_cast<Cache::TreeNode *>(index.internalPointer()), name);
}

void CacheModel::applyChanges()
{
	cache->flush();
}

void CacheModel::resetModel()
{
	beginResetModel();
	cache->reset();
	endResetModel();
}

}
