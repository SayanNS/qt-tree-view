//
// Created by sayan on 07.07.2022.
//

#ifndef BOOSTGRAPHLIBRARY_CACHEMODEL_H
#define BOOSTGRAPHLIBRARY_CACHEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include "Cache.h"


class CacheModel : public QAbstractItemModel
{
Q_OBJECT

public:
	explicit CacheModel(Cache &cache, QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	void loadNode(const QModelIndex &index);

	void addNode(const QModelIndex &index, QString name);

	void deleteNode(const QModelIndex &index);

	void applyChanges(QAbstractItemModel *model);

	void resetModel();

private:
	Cache &cache;
};


#endif //BOOSTGRAPHLIBRARY_CACHEMODEL_H
