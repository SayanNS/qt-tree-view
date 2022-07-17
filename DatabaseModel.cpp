//
// Created by sayan on 05.07.2022.
//

#include "DatabaseModel.h"
#include "Database.h"

namespace Mikran {

DatabaseModel::DatabaseModel(Database *t_database, QObject *parent)
	: QAbstractItemModel(parent)
	, m_database(t_database)
{
	m_database->setCallbacks
	(
			[this] (int parent_row, int child_row, Database::TreeNodeDescriptor parent) {
				// I know we should update our internal storage inside begin and end scope but in our case
				// it is pretty fine cause we add new items at the end of node child list
				beginInsertRows(createIndex(parent_row, 0, parent), child_row, child_row);
				endInsertRows();
			},
			[this] (int row, Database::TreeNodeDescriptor node) {
				QModelIndex index = createIndex(row, 0, node);
				emit dataChanged(index, index, {Qt::DisplayRole});
			}
	);
}

QVariant DatabaseModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	std::string data = m_database->getData(index.internalPointer()).name;

	return data.c_str();
}

Qt::ItemFlags DatabaseModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	if (m_database->getData(index.internalPointer()).deleted)
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
		return createIndex(row, column, m_database->getRoot());

	int counter = 0;

	for (auto node : boost::make_iterator_range(m_database->getChildrenIterator(parent.internalPointer()))) {
		if (counter == row) {
			return createIndex(row, column, node);
		}
		counter++;
	}
	return QModelIndex();
}

QModelIndex DatabaseModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	if (m_database->getRoot() == index.internalPointer())
		return QModelIndex();

	Database::TreeNodeDescriptor parent = m_database->getParent(index.internalPointer());

	if (parent == m_database->getRoot()) {
		return createIndex(0, 0, m_database->getRoot());
	}

	return createIndex(m_database->getData(parent).row, 0, parent);
}

int DatabaseModel::rowCount(const QModelIndex &parent) const
{
	Database::TreeNodeDescriptor node;

	if (parent.isValid()) {
		node = parent.internalPointer();
		return m_database->getData(node).count;
	} else {
		if (m_database->getRoot() == nullptr)
			return 0;
		return 1;
	}
}

int DatabaseModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

void DatabaseModel::resetModel()
{
	beginResetModel();
	m_database->reset();
	endResetModel();
}

}
