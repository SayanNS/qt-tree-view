//
// Created by sayan on 05.07.2022.
//

#include "DatabaseModel.h"
#include "Database.h"


namespace Mikran {

DatabaseModel::DatabaseModel(Database *t_database, QObject *parent)
	: QAbstractItemModel(parent)
	, database(t_database)
{
	database->setOnNewNodeAboutToBeInsertedHandler([this] (Database::TreeNode *parent, int position)
	{
		if (parent == this->database->getRoot()) {
			beginInsertRows(QModelIndex(), position, position);
		} else {
			beginInsertRows(createIndex(parent->getIndex(), 0, parent), position, position);
		}
	});

	database->setOnNewNodeInsertedHandler([this] ()
	{
		endInsertRows();
	});

	database->setOnNodeDataChangedHandler([this] (Database::TreeNode *node)
	{
		QModelIndex index = createIndex(node->getIndex(), 0, node);
		emit dataChanged(index, index, {Qt::DisplayRole});
	});
}

QVariant DatabaseModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	return static_cast<Database::TreeNode *>(index.internalPointer())->name;
}

Qt::ItemFlags DatabaseModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (static_cast<Database::TreeNode *>(index.internalPointer())->deleted)
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
		return createIndex(row, column, static_cast<void *>(database->getRoot()));

	Database::TreeNode *node = static_cast<Database::TreeNode *>(parent.internalPointer())->getChildrenAtIndex(row);

	return createIndex(row, column, node);
}

QModelIndex DatabaseModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	Database::TreeNode *node = static_cast<Database::TreeNode *>(index.internalPointer());
	Database::TreeNode *root = database->getRoot();

	if (root == node)
		return QModelIndex();

	Database::TreeNode *parent = node->getParent();

	if (root == parent)
		return createIndex(0, 0, parent);

	return createIndex(parent->getIndex(), 0, parent);
}

int DatabaseModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<Database::TreeNode *>(parent.internalPointer())->getChildrenCount();

	if (database->getRoot() == nullptr)
		return 0;

	return 1;
}

int DatabaseModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

void DatabaseModel::resetModel()
{
	beginResetModel();
	database->reset();
	endResetModel();
}

}
