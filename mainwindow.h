#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QAbstractItemModel;

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
	MainWindow(QAbstractItemModel *databaseModel, QAbstractItemModel *cacheModel, QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_newButton_clicked();

	void on_deleteButton_clicked();

	void on_loadButton_clicked();

	void on_applyButton_clicked();

	void on_resetButton_clicked();

private:
	Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
