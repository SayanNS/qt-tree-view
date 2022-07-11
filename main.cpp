#include "mainwindow.h"
#include "DatabaseModel.h"
#include "Database.h"
#include "Cache.h"
#include "CacheModel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Database database;
	DatabaseModel databaseModel(database);
	Cache cache(database);
	CacheModel cacheModel(cache);

	MainWindow w(&databaseModel, &cacheModel);
	w.show();
	return QApplication::exec();
}
