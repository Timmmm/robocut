#pragma once

#include <variant>

#include <QMainWindow>

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTimer>

#include "CutDialog.h"
#include "MeasureItem.h"
#include "PathScene.h"
#include "PathSorter.h"
#include "SvgPreviewModel.h"
#include "VinylCutterItem.h"

namespace Ui
{
class MainWindow;
}

// Position of the cut marker used for the animate feature.
struct CutMarkerPos
{
	// Current polygon (or polyline), or gap between them. E.g. if there are two shapes:
	// 0 is the line from the start position to the first shape.
	// 1 is the first shape.
	// 2 is the line from the first shape to the second shape.
	// 3 is the second shape.
	// 4 is the line from the second shape back to the start position.
	int poly = 0;
	// Current line in the polygon/line that is being cut.
	int line = 0;
	// Current distance along line.
	double distance = 0.0;
};

// Store data for when we are in the welcome page.
struct StateWelcome
{
	// No data
};

// Stores data for when the UI is in the file-loaded state.
struct StateFileLoaded
{
	// Filename it was loaded from.
	QString filename;
	// The polygons to cut, in mm.
	QList<QPolygonF> cutpaths;
	QList<QPolygonF> showpaths;
	// The page size in mm.
	QSizeF mediaSize;
	// The default zoom for this document, which is based on its size.
	double defaultZoom = 1.0;

	// The sorted paths.
	QList<QPolygonF> sortedCutPaths;
	QList<QPolygonF> sortedShowPaths;

	// Position of the cut marker for the animate feature.
	CutMarkerPos cutMarkerPos;

	QRectF markPosition;
	QPointF markOffset;
	double markStroke;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;

private slots:
	void on_actionAbout_triggered();
	void on_actionAnimate_toggled(bool animate);
	void on_actionClose_triggered();
	void on_actionCut_triggered();
	void on_actionDimensions_toggled(bool enabled);
	void on_actionExport_HPGL_triggered();
	void on_actionExit_triggered();
	void on_actionGrid_toggled(bool enabled);
	void on_actionManual_triggered();
	void on_actionOpen_triggered();
	void on_actionReload_triggered();
	void on_actionReset_triggered();
	void on_actionRulers_toggled(bool enabled);
	void on_actionZoom_In_triggered();
	void on_actionZoom_Out_triggered();
	void on_examplesList_activated(const QModelIndex& index);
	void on_openSvgButton_clicked();
	void on_recentFilesList_activated(const QModelIndex& index);

	// Advance the cutting animation frame.
	void animate();

	void onSortMethodTriggered(QAction* action);
	void onToolTriggered(QAction* action);

	void on_actionCutter_Path_toggled(bool enabled);
	void on_actionVinyl_Cutter_toggled(bool enabled);

	void onMouseMoved(QPointF pos);
	void onMousePressed(QPointF pos);

private:
	// Attempt to load the given file.
	void loadFile(QString filename);

	// Update the UI according to the current value of `state`.
	// Set window title, enable/disable actions etc.
	void updateUI();

	// Add the path items to the graphics scene for `data.paths`.
	void addPathItemsToScene();

	// Clear the scene and set all scene item pointers to null.
	void clearScene();

private:
	Ui::MainWindow* ui = nullptr;

	// Contains the data for when the file is loaded (if we are in the
	// file loaded state.
	std::variant<StateWelcome, StateFileLoaded> state = StateWelcome{};

	// The graphics scene which holds the cuts, the grid, the dimensions, etc.
	PathScene* scene = nullptr;

	// The dialog that asks what settings to use. We keep this around and reuse it as necessary.
	CutDialog* cutDialog = nullptr;

	// The dimensions item, e.g. "210 x 297 mm"
	QGraphicsTextItem* dimensionsItem = nullptr;

	// The grid item - the grid squares are children of this.
	QGraphicsItemGroup* gridItem = nullptr;

	// The rulers...
	//	QGraphicsItem*

	// The vinyl cutter, to make it clear where it will cut.
	VinylCutterItem* vinylCutterItem = nullptr;

	// The directory that the last file was opened from.
	QString lastOpenDir;

	// Timer for the cutting animation.
	QTimer* animationTimer = nullptr;
	// The circle that marks where the cutter blade is.
	QGraphicsItem* cutMarker = nullptr;

	// The lines that show the path the cutter takes.
	QGraphicsItemGroup* cutterPathItem = nullptr;

	// The actual cut polygons.
	QGraphicsItemGroup* pathsItem = nullptr;

	// Tape measure item
	MeasureItem* measureItem = nullptr;

	// List of recently loaded files.
	QStringList recentFiles;

	// Recently loaded files model.
	SvgPreviewModel* recentFilesModel = nullptr;

	// Example files model.
	SvgPreviewModel* exampleFilesModel = nullptr;

	// The current path sort method.
	PathSortMethod sortMethod = PathSortMethod::Best;

	bool rulersEnabled = true;
	bool gridEnabled = true;
	bool dimensionsEnabled = true;
	bool cutterPathEnabled = false;
	bool vinylCutterEnabled = true;

	enum class Tool
	{
		Pan,
		Measure,
	} tool = Tool::Pan;
};
