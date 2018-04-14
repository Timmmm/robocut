#pragma once

#include <QMainWindow>

#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsItem>

#include "CutDialog.h"
#include "SvgPreviewModel.h"
#include "PathSorter.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	
	
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
	void on_actionReload_triggered();
	void on_actionIdentify_triggered();

	// Advance the cutting animation frame.
	void animate();

	void on_openSvgButton_clicked();
	
	void on_actionClose_triggered();
	
	void on_recentFilesList_activated(const QModelIndex &index);
	
	void on_examplesList_activated(const QModelIndex &index);
	
	void on_sortMethod_triggered(QAction* action);
	
private:
	// Attempt to load the given file.
	void loadFile(QString currentFilename);
	
	// Set the currently loaded file. This updates the window title, menus etc.
	// Use an empty string to indicate that no file is loaded.
	void setFileLoaded(QString currentFilename);

private:
	Ui::MainWindow *ui;

	// The cutting paths that were loaded from the SVG.
	QList<QPolygonF> paths;

	// For displaying the cuts.
	QGraphicsScene* scene;

	// The dialog that asks what settings to use. We keep this around and reuse it as necessary.
	CutDialog* cutDialog = nullptr;

	// The directory that the last file was opened from.
	QString lastOpenDir;

	// Timer for the cutting animation.
	QTimer* animationTimer;
	// The circle that marks where the cutter blade is.
	QGraphicsItem* cutMarker = nullptr;
	// Cut marker progress, for animation.
	int cutMarkerPath; // Current path.
	int cutMarkerLine; // Current line in path
	double cutMarkerDistance; // Current distance along edge.

	// The page size in mm.
	QSizeF mediaSize;
	
	// TODO: In several places I just set the filename and then try to open an image - instead
	// there should be a proper open() method.
	// The currently loaded file. Empty if there isn't one.
	QString currentFilename;
	
	// List of recently loaded files.
	QStringList recentFiles;
	
	// Recently loaded files model.
	SvgPreviewModel* recentFilesModel;
	
	// Example files model.
	SvgPreviewModel* exampleFilesModel;
	
	// The current path sort method.
	PathSortMethod sortMethod = PathSortMethod::Best;

};
