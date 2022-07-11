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
	if (!ok || name.isEmpty()) return;

	CacheModel *treeModel = (CacheModel *)ui->cacheTreeView->model();
	treeModel->addNode(index, name);
}

void MainWindow::on_deleteButton_clicked()
{
	QModelIndex index = ui->cacheTreeView->currentIndex();
	if (!index.isValid()) return;

	CacheModel *cacheModel = reinterpret_cast<CacheModel *>(ui->cacheTreeView->model());
	cacheModel->deleteNode(index);

	while (index != QModelIndex() && !(index.flags() & Qt::ItemIsEnabled)) {
		index = index.parent();
	}
	ui->cacheTreeView->setCurrentIndex(index);
}

void MainWindow::on_loadButton_clicked()
{
	QModelIndex index = ui->databaseTreeView->currentIndex();
	if (!index.isValid()) return;

	CacheModel *cacheModel = reinterpret_cast<CacheModel *>(ui->cacheTreeView->model());
	cacheModel->loadNode(index);
}

void MainWindow::on_applyButton_clicked()
{
	CacheModel *cacheModel = reinterpret_cast<CacheModel *>(ui->cacheTreeView->model());
	cacheModel->applyChanges(ui->databaseTreeView->model());

	QModelIndex index = ui->databaseTreeView->currentIndex();
	if (!index.isValid()) return;

	while (index != QModelIndex() && !(index.flags() & Qt::ItemIsEnabled)) {
		index = index.parent();
	}
	ui->databaseTreeView->setCurrentIndex(index);
}

void MainWindow::on_resetButton_clicked()
{
	DatabaseModel *databaseModel = reinterpret_cast<DatabaseModel *>(ui->databaseTreeView->model());
	databaseModel->resetModel();
	CacheModel *cacheModel = reinterpret_cast<CacheModel *>(ui->cacheTreeView->model());
	cacheModel->resetModel();
}
