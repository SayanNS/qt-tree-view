//
// Created by sayan on 05.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_DATABASEMODEL_H
#define BOOSTGRAPHLIBRARY_DATABASEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include "Cache.h"


class DatabaseModel : public QAbstractItemModel
{
Q_OBJECT

public:
	explicit DatabaseModel(Database &database, QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Database::tree_node_descriptor createNode(Database::tree_node_descriptor parent,
			const cache_node_data &cacheNodeData);

	void deleteNode(Database::tree_node_descriptor node);

	void changeNodeData(const cache_node_data &cacheNodeData);

	void resetModel();

private:
	Database &database;
};


#endif //BOOSTGRAPHLIBRARY_DATABASEMODEL_H
