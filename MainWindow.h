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

	// The cutting paths that were loaded from the SVG.
	QList<QPolygonF> paths;

	// For displaying the cuts.
	QGraphicsScene* scene;

	// The dialog that asks what settings to use. We keep this around and reuse it as necessary.
	CutDialog* cutDialog;

	// The directory that the last file was opened from.
	QString lastOpenDir;

	// Timer for the cutting animation.
	QTimer* animationTimer;
	// The circle that marks where the cutter blade is.
	QGraphicsItem* cutMarker;
	// Cut marker progress.
	int cutMarkerPath; // Current path.
	int cutMarkerLine; // Current line in path
	double cutMarkerDistance; // Current distance along edge.

	QSizeF mediaSize;
	
	QString filename;

public:
	int sortFlag;
	int tspFlag;
	int cutFlag;
	char *fileValue;

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

private:
	// Use empty string to indicate no file is loaded.
	void setFileLoaded(QString filename);
	bool eventFilter(QObject *o, QEvent *e);
	void loadFile();
};

#endif // MAINWINDOW_H
