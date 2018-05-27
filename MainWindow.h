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
	void on_actionAbout_triggered();
	void on_actionAnimate_toggled(bool animate);
	void on_actionClose_triggered();
	void on_actionCut_triggered();
	void on_actionDimensions_toggled(bool enabled);
	void on_actionExit_triggered();
	void on_actionGrid_toggled(bool enabled);
	void on_actionIdentify_triggered();
	void on_actionManual_triggered();
	void on_actionOpen_triggered();
	void on_actionReload_triggered();
	void on_actionReset_triggered();
	void on_actionRulers_toggled(bool enabled);
	void on_actionVerify_Adjust_Scale_triggered();
	void on_actionZoom_In_triggered();
	void on_actionZoom_Out_triggered();
	void on_examplesList_activated(const QModelIndex &index);
	void on_openSvgButton_clicked();	
	void on_recentFilesList_activated(const QModelIndex &index);
	
	// Advance the cutting animation frame.
	void animate();
	
	void onSortMethodTriggered(QAction* action);
	
	
	void on_actionCutter_Path_toggled(bool enabled);
	
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

	// The graphics scene which holds the cuts, the grid, the dimensions, etc.
	QGraphicsScene* scene;

	// The dialog that asks what settings to use. We keep this around and reuse it as necessary.
	CutDialog* cutDialog = nullptr;
	
	// The dimensions item, e.g. "210 x 297 mm"
	QGraphicsTextItem* dimensionsItem = nullptr;
	
	// The grid item - the grid squares are children of this.
	QGraphicsItemGroup* gridItem = nullptr;
	
	// The rulers...
//	QGraphicsItem* 

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
	
	// The lines that show the path the cutter takes.
	QGraphicsItemGroup* cutterPathItem = nullptr;

	// The page size in mm.
	QSizeF mediaSize;
	
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
	
	bool rulersEnabled = true;
	bool gridEnabled = true;
	bool dimensionsEnabled = true;
	bool cutterPathEnabled = false;
	
	// The default zoom for the current document, which is based on its size.
	double defaultZoom = 1.0;

};
