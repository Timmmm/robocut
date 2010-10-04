#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QGraphicsScene>

#include "CutDialog.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;

	QList<QPolygonF> paths;

	QGraphicsScene* scene;

	CutDialog* cutDialog;

private slots:
	void on_actionPreview_triggered();
 void on_actionCut_triggered();
 void on_actionExit_triggered();
 void on_actionAbout_triggered();
 void on_actionOpen_triggered();
};

#endif // MAINWINDOW_H
