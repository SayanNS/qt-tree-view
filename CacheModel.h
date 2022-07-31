//
// Created by sayan on 07.07.2022.
//

#ifndef MIKRAN_CACHEMODEL_H
#define MIKRAN_CACHEMODEL_H

#include <QAbstractItemModel>


namespace Mikran {

class Cache;

class CacheModel : public QAbstractItemModel
{
Q_OBJECT

public:
	explicit CacheModel(Cache *cache, QObject *parent = nullptr);

	QVariant data(const QModelIndex &index, int role) const override;

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	Qt::ItemFlags flags(const QModelIndex &index) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	void loadIndex(const QModelIndex &index);

	void addIndex(const QModelIndex &index, QString &name);

	void deleteIndex(const QModelIndex &index);

	void applyChanges();

	void resetModel();

private:
	Cache *cache;
};

}

#endif //MIKRAN_CACHEMODEL_H
