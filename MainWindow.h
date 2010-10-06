#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsItem>

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

	QString lastOpenDir;

	QTimer* animationTimer;

	QGraphicsItem* cutMarker;
	// Cut marker progress.
	int cutMarkerPath; // Current path.
	int cutMarkerLine; // Current line in path
	double cutMarkerDistance; // Current distance along edge.

private slots:
	void on_actionZoom_Out_triggered();
	void on_actionZoom_In_triggered();
	void on_actionReset_triggered();
	void on_actionAnimate_toggled(bool animate);
	void on_actionManual_triggered();
	void on_actionCut_triggered();
	void on_actionExit_triggered();
	void on_actionAbout_triggered();
	void on_actionOpen_triggered();

	// Advance the cutting animation frame.
	void animate();
};

#endif // MAINWINDOW_H
