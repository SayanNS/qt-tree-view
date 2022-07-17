#include "mainwindow.h"
#include "Database.h"
#include "DatabaseModel.h"
#include "Cache.h"
#include "CacheModel.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Mikran::Database database;
	Mikran::DatabaseModel databaseModel(&database);
	Mikran::Cache cache(&database);
	Mikran::CacheModel cacheModel(&cache);

	MainWindow w(&databaseModel, &cacheModel);
	w.show();
	return QApplication::exec();
}
