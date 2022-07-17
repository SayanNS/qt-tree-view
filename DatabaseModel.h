//
// Created by sayan on 05.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_DATABASEMODEL_H
#define BOOSTGRAPHLIBRARY_DATABASEMODEL_H

#include <QAbstractItemModel>

namespace Mikran {

class Database;

class DatabaseModel : public QAbstractItemModel
{
Q_OBJECT

public:
	explicit DatabaseModel(Database *database, QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	void resetModel();

private:
	Database *m_database;
};

}

#endif //BOOSTGRAPHLIBRARY_DATABASEMODEL_H
