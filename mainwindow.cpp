#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "DatabaseModel.h"
#include "CacheModel.h"
#include <QInputDialog>

MainWindow::MainWindow(QAbstractItemModel *databaseModel, QAbstractItemModel *cacheModel, QWidget *parent)
		: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	ui->databaseTreeView->setModel(databaseModel);
	ui->cacheTreeView->setModel(cacheModel);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_newButton_clicked()
{
	QModelIndex index = ui->cacheTreeView->currentIndex();
	if (!index.isValid()) return;

	bool ok;
	QString name = QInputDialog::getText(this, "Name", "Enter a name",
			QLineEdit::Normal, QString(), &ok);

	if (!ok || name.isEmpty())
		return;

	Mikran::CacheModel *cacheModel = static_cast<Mikran::CacheModel *>(ui->cacheTreeView->model());
	cacheModel->addIndex(index, name);
}

void MainWindow::on_deleteButton_clicked()
{
	QModelIndex index = ui->cacheTreeView->currentIndex();
	if (!index.isValid()) return;

	Mikran::CacheModel *cacheModel = static_cast<Mikran::CacheModel *>(ui->cacheTreeView->model());
	cacheModel->deleteIndex(index);

	ui->cacheTreeView->setCurrentIndex(QModelIndex());
}

void MainWindow::on_loadButton_clicked()
{
	QModelIndex index = ui->databaseTreeView->currentIndex();

	if (!index.isValid())
		return;

	Mikran::CacheModel *cacheModel = static_cast<Mikran::CacheModel *>(ui->cacheTreeView->model());
	cacheModel->loadIndex(index);
}

void MainWindow::on_applyButton_clicked()
{
	Mikran::CacheModel *cacheModel = static_cast<Mikran::CacheModel *>(ui->cacheTreeView->model());
	cacheModel->applyChanges();

	QModelIndex index = ui->databaseTreeView->currentIndex();
	if (index != QModelIndex() && !(index.flags() & Qt::ItemIsEnabled)) {
		ui->databaseTreeView->setCurrentIndex(QModelIndex());
	}
}

void MainWindow::on_resetButton_clicked()
{
	Mikran::DatabaseModel *databaseModel = static_cast<Mikran::DatabaseModel *>(ui->databaseTreeView->model());
	Mikran::CacheModel *cacheModel = static_cast<Mikran::CacheModel *>(ui->cacheTreeView->model());

	databaseModel->resetModel();
	cacheModel->resetModel();
}
