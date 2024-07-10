#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuttingDialog.h"
#include "HPGL2.h"
#include "PathPaintDevice.h"
#include "PathSorter.h"
#include "Plotter.h"
#include "SvgRenderer.h"

#include <cmath>
#include <iostream>

#include <QActionGroup>
#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSaveFile>
#include <QSettings>
#include <QShortcut>
#include <QSvgRenderer>

#include <algorithm>

using namespace std;

namespace
{

void removeNonexistentFiles(QStringList& filenames)
{
	filenames.erase(std::remove_if(filenames.begin(), filenames.end(), [](const QString& filename) { return !QFile::exists(filename); }), filenames.end());
}

} // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	// Load settings from disk.
	QSettings settings;
	lastOpenDir = settings.value("lastOpenDir").toString();
	recentFiles = settings.value("recentFiles").toStringList();
	gridEnabled = settings.value("gridEnabled", true).toBool();
	dimensionsEnabled = settings.value("dimensionsEnabled", true).toBool();
	rulersEnabled = settings.value("rulersEnabled", true).toBool();
	cutterPathEnabled = settings.value("cutterPathEnabled", false).toBool();
	vinylCutterEnabled = settings.value("vinylCutterEnabled", true).toBool();

	ui->actionGrid->setChecked(gridEnabled);
	ui->actionDimensions->setChecked(dimensionsEnabled);
	ui->actionRulers->setChecked(rulersEnabled);
	ui->actionCutter_Path->setChecked(cutterPathEnabled);
	ui->actionVinyl_Cutter->setChecked(vinylCutterEnabled);

	// TODO: Implement this.
	//	ui->menuEdit->hide();

	// TODO: Implement this.
	ui->devicesWidget->hide();

	scene = new PathScene(this);

	ui->graphicsView->setScene(scene);

	animationTimer = new QTimer(this);
	connect(animationTimer, &QTimer::timeout, this, &MainWindow::animate);

	removeNonexistentFiles(recentFiles);
	recentFilesModel = new SvgPreviewModel(this);
	recentFilesModel->setPreviewSize(64);
	recentFilesModel->setFiles(recentFiles);
	ui->recentFilesList->setModel(recentFilesModel);

	exampleFilesModel = new SvgPreviewModel(this);
	exampleFilesModel->setPreviewSize(64);

	auto exampleFiles = QDir(":/examples").entryList(QDir::Files, QDir::Name);
	for (auto& f : exampleFiles)
		f.prepend(":/examples/");
	exampleFilesModel->setFiles(exampleFiles);

	ui->examplesList->setModel(exampleFilesModel);

	// Alternative zoom shortcuts
	QShortcut* zoom_in = new QShortcut(QKeySequence("X"), this);
	connect(zoom_in, &QShortcut::activated, this, &MainWindow::on_actionZoom_In_triggered);
	QShortcut* zoom_out = new QShortcut(QKeySequence("Z"), this);
	connect(zoom_out, &QShortcut::activated, this, &MainWindow::on_actionZoom_Out_triggered);

	auto toolGroup = new QActionGroup(this);
	toolGroup->addAction(ui->actionPan);
	toolGroup->addAction(ui->actionMeasure);

	connect(toolGroup, &QActionGroup::triggered, this, &MainWindow::onToolTriggered);

	auto sortMethodGroup = new QActionGroup(this);
	sortMethodGroup->addAction(ui->actionSortShortest);
	sortMethodGroup->addAction(ui->actionSortByY);
	sortMethodGroup->addAction(ui->actionSortGreedy);
	sortMethodGroup->addAction(ui->actionSortInsideFirst);
	sortMethodGroup->addAction(ui->actionSortNone);

	connect(sortMethodGroup, &QActionGroup::triggered, this, &MainWindow::onSortMethodTriggered);

	// Initialise icons.
	// Dark icons generated with:
	//
	//   for FILE in *.svg ; do cat $FILE | sed -e 's/path/path fill="white"/g' > dark_$FILE ; done
	//

	auto background = QApplication::palette("QWidget").color(QPalette::Base);
	auto foreground = QApplication::palette("QWidget").color(QPalette::Text);

	bool isDarkMode = foreground.lightness() > background.lightness();

	auto assignIcon = [&](QAction* action, QString on, QString off = "") {
		QIcon icon;
		if (off.isEmpty())
		{
			icon.addFile((isDarkMode ? ":icons/dark_ic_" : ":icons/ic_") + on + "_48px.svg");
		}
		else
		{
			icon.addFile(
			    (isDarkMode ? ":icons/dark_ic_" : ":icons/ic_") + on + "_48px.svg",
			    QSize(),
			    QIcon::Normal,
			    QIcon::On);
			icon.addFile(
			    (isDarkMode ? ":icons/dark_ic_" : ":icons/ic_") + off + "_48px.svg",
			    QSize(),
			    QIcon::Normal,
			    QIcon::Off);
			// TODO: Probably need to add it for disabled, etc. too.
		}
		icon.setIsMask(true);
		action->setIcon(icon);
	};

	assignIcon(ui->actionOpen, "folder_open");
	assignIcon(ui->actionCut, "print");
	assignIcon(ui->actionClose, "close");
	assignIcon(ui->actionGrid, "grid_on", "grid_off");
	assignIcon(ui->actionCutter_Path, "timeline", "timeline_off");
	assignIcon(ui->actionAnimate, "play_arrow");
	assignIcon(ui->actionPan, "pan_tool");
	assignIcon(ui->actionMeasure, "straighten");

	// Receive mouse move events because in "measure" mode we move the measure item
	// to follow the mouse (with snapping).
	connect(scene, &PathScene::mouseMoved, this, &MainWindow::onMouseMoved);
	connect(scene, &PathScene::mousePressed, this, &MainWindow::onMousePressed);
}

MainWindow::~MainWindow()
{
	delete ui;

	QSettings settings;
	settings.setValue("lastOpenDir", lastOpenDir);
	settings.setValue("recentFiles", recentFiles);
	settings.setValue("gridEnabled", gridEnabled);
	settings.setValue("dimensionsEnabled", dimensionsEnabled);
	settings.setValue("rulersEnabled", rulersEnabled);
	settings.setValue("cutterPathEnabled", cutterPathEnabled);
	settings.setValue("vinylCutterEnabled", vinylCutterEnabled);
}

void MainWindow::on_actionOpen_triggered()
{
	if (lastOpenDir.isEmpty())
		lastOpenDir = QDir::homePath();

	auto filename = QFileDialog::getOpenFileName(this, tr("Open File"), lastOpenDir, tr("SVG Files (*.svg)"));

	if (filename.isEmpty())
		return;

	lastOpenDir = QFileInfo(filename).absoluteDir().path();
	loadFile(filename);
}

void MainWindow::on_actionReload_triggered()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Reload triggered with no file loaded";
		return;
	}

	if (!QFile::exists(s->filename))
	{
		qDebug() << "Reload failed. File missing: " << s->filename;
		return;
	}

	loadFile(s->filename);
}

void MainWindow::loadFile(QString filename)
{
	qDebug() << "Reading file: " << filename;

	// Convert the SVG to paths.
	auto renderResult = svgToPaths(filename, true);

	if (renderResult.is_err())
	{
		QMessageBox::critical(this, "Error loading file.", QString::fromStdString(renderResult.unwrap_err()));
		return;
	}

	auto render = renderResult.unwrap();

	if (render.hasTspanPosition)
	{
		QMessageBox::warning(
		    this,
		    "Multi-line Text",
		    "This SVG contains multi-line text which "
		    "is not supported in SVG Tiny. Text may be rendered incorrectly. "
		    "The easiest workaround is to convert the text to paths.");
	}

	StateFileLoaded loadedState;

	// When Qt renders an SVG it sets its width and height based on the
	// width="" height="" svg attributes. These can be given in physical units
	// e.g. width="25mm". In that case Qt renders at 90 DPI. However that is
	// incorrect. SVG/CSS use 96 DPI.
	//
	// See `convertToPixels()` here: https://codebrowser.dev/qt6/qtsvg/src/svg/qsvghandler.cpp.html#932
	//

	// This is the size of the entire document in mm, calculated from the
	// width/height attributes, using the correct conversion from pixels (96 DPI).
	QSizeF mediaSize(render.widthMm, render.heightMm);

	loadedState.mediaSize = mediaSize;
	loadedState.showpaths = std::move(render.showpaths);
	loadedState.cutpaths = std::move(render.cutpaths);
	loadedState.filename = filename;
	loadedState.markPosition = render.markPosition;
	loadedState.markOffset = render.markOffset;
	loadedState.markStroke = render.markStroke;
	QPointF startingPoint(0.0, 0.0);

	// Sort the paths with the following goals:
	//
	// 1. Minimise the time spend travelling without cutting.
	// 2. Try to cut inside shapes before outside shapes.
	// 3. Try not to travel too much in the Y direction (it can lead to accumulation of
	//    errors due to the vinyl slipping in the rollers).
	//
	loadedState.sortedCutPaths = sortPaths(loadedState.cutpaths, sortMethod, startingPoint);
	loadedState.sortedShowPaths = loadedState.showpaths;

	// Set the default zoom.
	auto viewSize = ui->centralWidget->size();
	loadedState.defaultZoom =
	    0.8 * std::min(viewSize.width() / mediaSize.width(), viewSize.height() / mediaSize.height());

	state = std::move(loadedState);

	// Clear the scene.
	clearScene();

	QRectF pageRect(0.0, 0.0, mediaSize.width()*96/90, mediaSize.height()*96/90);

	// And reset the sceneRect, which it doesn't do by default.
	scene->setSceneRect(pageRect.adjusted(-20, -20, 20, 20));

	scene->setBackgroundBrush(QBrush(Qt::lightGray));

	// Add the page background
	scene->addRect(pageRect, Qt::NoPen, QBrush(Qt::white));

	// Add the dimensions test.
	dimensionsItem = scene->addText(
	    QString::number(mediaSize.width()) + " × " + QString::number(mediaSize.height()) + " mm",
	    QFont("Helvetica", 10));
	dimensionsItem->setPos(0.0, mediaSize.height()*96/90);
	dimensionsItem->setVisible(dimensionsEnabled);

	// Add the rulers.
	//	addRulers(scene);

	// Add the measuring tape and hide it.
	measureItem = new MeasureItem();
	measureItem->setZValue(10);
	measureItem->hide();
	scene->addItem(measureItem);

	QList<QGraphicsItem*> gridSquares;

	QPen mmPen = QPen(QBrush(QColor::fromRgb(240, 240, 240)), 0.0);
	QPen cmPen = QPen(QBrush(QColor::fromRgb(200, 200, 200)), 0.0);
	for (int x=0; x<=mediaSize.width()*96/90; x++)
    gridSquares.append(scene->addLine(x,0,x,mediaSize.height()*96/90, (x%10==0)?cmPen:mmPen));
	for (int y=0; y<=mediaSize.height()*96/90; y++)
    gridSquares.append(scene->addLine(0,y,mediaSize.width()*96/90,y, (y%10==0)?cmPen:mmPen));

	gridItem = scene->createItemGroup(gridSquares);
	gridItem->setVisible(gridEnabled);

	// Add the outline of the page.
	QPen pageOutlinePen;
	pageOutlinePen.setCosmetic(true);
	scene->addRect(pageRect, pageOutlinePen, Qt::NoBrush);

	// Add the vinyl cutter item.
	vinylCutterItem = new VinylCutterItem();
	vinylCutterItem->setMediaWidth(static_cast<int>(mediaSize.width()));
	vinylCutterItem->setVisible(vinylCutterEnabled);
	scene->addItem(vinylCutterItem);

	addPathItemsToScene();

	// Handle the animation. I.e. stop it.
	// The old one was deleted when we cleared the scene.
	cutMarker = scene->addEllipse(-1.0, -1.0, 2.0, 2.0, QPen(Qt::black), QBrush(Qt::red));
	cutMarker->hide();

	ui->actionAnimate->setChecked(false);

	// Reset the viewport.
	on_actionReset_triggered();

	// Set the page that shows the file.
	ui->stackedWidget->setCurrentIndex(1);

	// Redraw. Probably not necessary.
	update();

	// TODO: Show a non-modal warning at the top.
	//	if (clipped)
	//		QMessageBox::warning(this, "Paths clipped", "<b>WARNING!</b><br><br>Some paths lay outside the
	// 210&times;297&thinsp;mm A4 area. These have been squeezed back onto the page in a most ugly fashion, so cutting
	// will almost certainly not do what you want.");

	// Change window title and enable menu items.
	updateUI();
}

void MainWindow::on_actionAbout_triggered()
{
	QString message =
	    QString("<b>") + PROJECT_VERSION + "</b><br><br>By Tim Hutt, &copy; 2010-2022 "
	    "with contributiosn from Markus Schulz and others."
	    "<br/><br/>"
	    "This software allows you to read a vector image in <a "
	    "href=\"http://en.wikipedia.org/wiki/Scalable_Vector_Graphics\">SVG format</a>, "
	    "and send it to a <a href=\"http://www.graphteccorp.com/\">Graphtec</a> vinyl cutter. "
	    "See <a href=\"http://robocut.org/\">robocut.org</a> for more information.";
	QMessageBox::information(this, "About", message);
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionCut_triggered()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Cut triggered with no file loaded";
		return;
	}

	if (!cutDialog)
		cutDialog = new CutDialog(this);

	if (!s->markPosition.isNull()) std::cout << "Found regmarks in SVG!" << std::endl;
	cutDialog->setRegMarks(s->markPosition);

	if (cutDialog->exec() != QDialog::Accepted)
		return;

	// Create a new dialog and run the actual cutting in a different thread.

	CuttingDialog* cuttingDlg = new CuttingDialog(this);

	CutParams params;
	params.cuts = s->sortedCutPaths;
	params.mediawidth = s->mediaSize.width();
	params.mediaheight = s->mediaSize.height();
	params.media = cutDialog->media();
	params.pressure = cutDialog->pressure();
	params.regwidth = cutDialog->regWidth();
	params.regheight = cutDialog->regHeight();
	params.regmark = cutDialog->regMark();
	params.regoffset = s->markOffset;
	params.regsearch = cutDialog->regSearch();
	params.regstroke = s->markStroke;
	params.speed = cutDialog->speed();
	params.trackenhancing = cutDialog->trackEnhancing();

	cuttingDlg->startCut(params);
	cuttingDlg->show();
}

void MainWindow::on_actionManual_triggered()
{
	QMessageBox::information(
	    this,
	    "Manual",
	    "An online manual is available at <br><br><a href=\"http://robocut.org/\">http://robocut.org/</a>");
}

void MainWindow::on_actionAnimate_toggled(bool animate)
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Animate triggered with no file loaded";
		return;
	}

	s->cutMarkerPos.poly = 0;
	s->cutMarkerPos.line = 0;
	s->cutMarkerPos.distance = 0.0;

	if (animate)
	{
		animationTimer->start(30);
		if (cutMarker)
		{
			cutMarker->setPos(0, 0);
			cutMarker->show();
		}
	}
	else
	{
		animationTimer->stop();
		if (cutMarker)
			cutMarker->hide();
	}
}

void MainWindow::on_actionReset_triggered()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Reset triggered with no file loaded";
		return;
	}
	ui->graphicsView->resetTransform();
	ui->graphicsView->scale(s->defaultZoom, s->defaultZoom);
}

void MainWindow::on_actionZoom_In_triggered()
{
	ui->graphicsView->scale(1.2, 1.2);
}

void MainWindow::on_actionZoom_Out_triggered()
{
	ui->graphicsView->scale(1.0 / 1.2, 1.0 / 1.2);
}

void MainWindow::animate()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
		return;

	// TODO: This method is a bit of a mess - could do with moving the
	// iteration stuff into a separate iterator class.

	if (!cutMarker)
		return;

	auto& m = s->cutMarkerPos;

	// Make sure the current position is sane.
	if (m.poly >= s->sortedCutPaths.size() * 2 + 1)
	{
		// If not, reset it.
		m.poly = 0;
		m.line = 0;
		m.distance = 0.0;
		return;
	}
	// Get the ends of the segment/edge we are currently on.

	QPointF startingPoint(0.0, 0.0);

	// How far to travel each "frame". This controls the animation speed.
	double distanceRemaining = 2.0;

	while (true)
	{
		// Start and end points of the current line.
		QPointF a;
		QPointF b;

		if (m.poly % 2 == 0)
		{
			// We are moving between paths, and not cutting.
			if (m.poly == 0)
				a = startingPoint;
			else
				a = s->sortedCutPaths[(m.poly / 2) - 1].back();

			if (m.poly >= s->sortedCutPaths.size() * 2)
				b = startingPoint;
			else
				b = s->sortedCutPaths[(m.poly / 2)].front();

			cutMarker->setOpacity(0.2);
		}
		else
		{
			auto pathIndex = (m.poly - 1) / 2;
			a = s->sortedCutPaths[pathIndex][m.line];
			b = s->sortedCutPaths[pathIndex][m.line + 1];
			cutMarker->setOpacity(1.0);
		}

		QLineF ln(a, b);

		// If the cutter shouldn't be in this line, go to the next one.
		if ((ln.length() - m.distance) < distanceRemaining)
		{
			distanceRemaining -= (ln.length() - m.distance);
			m.distance = 0.0;
			if (m.poly % 2 != 0 && m.line < s->sortedCutPaths[(m.poly - 1) / 2].size() - 2)
			{
				++m.line;
			}
			else
			{
				m.poly = (m.poly + 1) % (s->sortedCutPaths.size() * 2 + 1);
				m.line = 0;
			}
			continue;
		}

		m.distance += distanceRemaining;

		// Get a vector along the current line.
		QPointF r = b - a;
		double h = hypot(r.x(), r.y());
		if (h > 0.0)
			r /= h;

		// The current position of the marker.
		QPointF p = a + r * m.distance;

		cutMarker->setPos(p);
		break;
	}
}

void MainWindow::updateUI()
{
	auto s = std::get_if<StateFileLoaded>(&state);

	bool e = s != nullptr;

	// Set window title.
	if (e)
		setWindowTitle("Robocut - " + s->filename);
	else
		setWindowTitle("Robocut");

	// Enable/disable menu actions.
	ui->actionAnimate->setEnabled(e);
	ui->actionReset->setEnabled(e);
	ui->actionZoom_In->setEnabled(e);
	ui->actionZoom_Out->setEnabled(e);
	ui->actionCut->setEnabled(e);
	ui->actionReload->setEnabled(e);
	ui->actionClose->setEnabled(e);
	ui->actionPan->setEnabled(e);
	ui->actionMeasure->setEnabled(e);
	ui->actionExport_HPGL->setEnabled(e);

	// Ensure the current file is in the recent files list.
	// Ignore files that start with a colon - those are examples.
	if (e && !s->filename.startsWith(":"))
	{
		recentFiles.removeAll(s->filename);
		recentFiles.prepend(s->filename);
	}
}

void MainWindow::addPathItemsToScene()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Add path items to scene with no file loaded";
		return;
	}

	if (pathsItem != nullptr)
	{
		delete pathsItem;
		pathsItem = nullptr;
	}
	if (cutterPathItem != nullptr)
	{
		delete cutterPathItem;
		cutterPathItem = nullptr;
	}

	// Add lines for the actual shapes to be cut out. These are always shown.
	double hue = 0.0;
	const double golden = 0.5 * (1.0 + sqrt(5.0));

	QList<QGraphicsItem*> pathLines;

	for (const auto& path : s->sortedShowPaths)
	{
		QPen pen(QColor::fromHsvF(static_cast<float>(hue), 1.0f, 0.7f));

		// Don't change the pen width with zoom.
		pen.setCosmetic(true);
		pen.setWidthF(3.0);

		// Add the path to the painter path, but offset to account for the viewBox position.
		QPainterPath pp;
		pp.addPolygon(path);
		// addPath() is used rather than addPolygon() because addPolygon() is always closed.
		pathLines.append(scene->addPath(pp, pen));

		hue += golden;
		hue -= trunc(hue);
	}

	pathsItem = scene->createItemGroup(pathLines);

	// Add the lines that show where the cutter goes between shapes in light grey.
	// These can be hidden.
	QList<QGraphicsItem*> cutterPathLines;

	QPointF startingPoint(0.0, 0.0);

	QPointF currentPoint = startingPoint;
	QPen pen(QColor::fromHsvF(0.0f, 0.0f, 0.8f));
	// Don't change the pen width with zoom.
	pen.setCosmetic(true);
	pen.setWidthF(3.0);
	for (const auto& path : s->sortedShowPaths)
	{
		cutterPathLines.append(scene->addLine(QLineF(currentPoint, path.first()), pen));
		currentPoint = path.last();
	}

	cutterPathItem = scene->createItemGroup(cutterPathLines);
	cutterPathItem->setVisible(cutterPathEnabled);
}

void MainWindow::clearScene()
{
	scene->clear();

	// The following items are all deleted by clearing the scene,
	// so set them to null.
	dimensionsItem = nullptr;
	gridItem = nullptr;
	cutMarker = nullptr;
	cutterPathItem = nullptr;
	pathsItem = nullptr;
	measureItem = nullptr;
	vinylCutterItem = nullptr;
}

void MainWindow::on_openSvgButton_clicked()
{
	on_actionOpen_triggered();
}

void MainWindow::on_actionClose_triggered()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Close triggered with no file loaded";
		return;
	}

	state = StateWelcome{};
	updateUI();

	clearScene();

	ui->stackedWidget->setCurrentIndex(0);

	// Update the recent files model - a new one may have been loaded. Unfortunately
	// this re-renders them all, but eh.
	// TODO: Don't re-render them all.
	recentFilesModel->setFiles(recentFiles);
}

void MainWindow::on_recentFilesList_activated(const QModelIndex& index)
{
	auto filename = index.data(SvgPreviewModel::FilenameRole).toString();

	lastOpenDir = QFileInfo(filename).absoluteDir().path();
	loadFile(filename);
}

void MainWindow::on_examplesList_activated(const QModelIndex& index)
{
	auto filename = index.data(SvgPreviewModel::FilenameRole).toString();

	loadFile(filename);
}

void MainWindow::onSortMethodTriggered(QAction* action)
{
	if (action == ui->actionSortGreedy)
		sortMethod = PathSortMethod::Greedy;
	else if (action == ui->actionSortByY)
		sortMethod = PathSortMethod::IncreasingY;
	else if (action == ui->actionSortInsideFirst)
		sortMethod = PathSortMethod::InsideFirst;
	else if (action == ui->actionSortShortest)
		sortMethod = PathSortMethod::Best;
	else if (action == ui->actionSortNone)
		sortMethod = PathSortMethod::None;
	else
		return;

	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
		return;

	// Resort the paths.
	QPointF startingPoint(0.0, 0.0);
	s->sortedCutPaths = sortPaths(s->cutpaths, sortMethod, startingPoint);

	// Remove the path items and re-add them.
	addPathItemsToScene();
}

void MainWindow::onToolTriggered(QAction* action)
{
	if (action == ui->actionPan)
	{
		tool = Tool::Pan;

		// Change the cursor. TODO: https://stackoverflow.com/a/10953718/265521
		ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
		ui->graphicsView->setCursor(Qt::OpenHandCursor);

		// Hide the measuring tape item
		if (measureItem != nullptr)
		{
			measureItem->hide();
		}
	}
	else if (action == ui->actionMeasure)
	{
		tool = Tool::Measure;

		// Change the cursor. TODO: https://stackoverflow.com/a/10953718/265521
		ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
		ui->graphicsView->setCursor(Qt::CrossCursor);
		if (measureItem != nullptr)
		{
			measureItem->setState(MeasureItem::State::Free);
			measureItem->setPos(QPointF());
			measureItem->setVec(QPointF());
		}
	}
}

void MainWindow::on_actionRulers_toggled(bool enabled)
{
	// TODO: Implement rulers.
	rulersEnabled = enabled;
	ui->centralWidget->update();
}

void MainWindow::on_actionGrid_toggled(bool enabled)
{
	gridEnabled = enabled;
	if (gridItem != nullptr)
		gridItem->setVisible(gridEnabled);
}

void MainWindow::on_actionDimensions_toggled(bool enabled)
{
	dimensionsEnabled = enabled;
	if (dimensionsItem != nullptr)
		dimensionsItem->setVisible(dimensionsEnabled);
}

void MainWindow::on_actionCutter_Path_toggled(bool enabled)
{
	cutterPathEnabled = enabled;
	if (cutterPathItem != nullptr)
		cutterPathItem->setVisible(cutterPathEnabled);
}

void MainWindow::on_actionVinyl_Cutter_toggled(bool enabled)
{
	vinylCutterEnabled = enabled;
	if (vinylCutterItem != nullptr)
		vinylCutterItem->setVisible(vinylCutterEnabled);
}

void MainWindow::onMouseMoved(QPointF pos)
{
	if (tool == Tool::Measure)
	{
		if (measureItem != nullptr)
		{
			// Show the measure item.
			measureItem->show();
			switch (measureItem->state())
			{
			case MeasureItem::State::Free:
				measureItem->setPos(pos);
				measureItem->setVec(QPointF(30.0, 0.0));
				scene->update();
				break;
			case MeasureItem::State::OneEndAttached:
				measureItem->setVec(pos - measureItem->pos());
				scene->update();
				break;
			case MeasureItem::State::BothEndsAttached:
				break;
			}
		}
	}
}

void MainWindow::onMousePressed(QPointF pos)
{
	if (tool == Tool::Measure)
	{
		if (measureItem != nullptr)
		{
			switch (measureItem->state())
			{
			case MeasureItem::State::Free:
				measureItem->setState(MeasureItem::State::OneEndAttached);
				measureItem->setPos(pos);
				measureItem->setVec(QPointF());
				break;
			case MeasureItem::State::OneEndAttached:
				measureItem->setState(MeasureItem::State::BothEndsAttached);
				measureItem->setVec(pos - measureItem->pos());
				break;
			case MeasureItem::State::BothEndsAttached:
				measureItem->setState(MeasureItem::State::Free);
				measureItem->setPos(pos);
				measureItem->setVec(QPointF(30.0, 0.0));
				break;
			}
		}
	}
}

void MainWindow::on_actionExport_HPGL_triggered()
{
	auto s = std::get_if<StateFileLoaded>(&state);
	if (s == nullptr)
	{
		qDebug() << "Export HPGL triggered with no file loaded";
		return;
	}

	// Export HPGL2.
	auto filename = QFileDialog::getSaveFileName(this, tr("Export HPGL2"), lastOpenDir, tr("HPGL Files (*.hpgl)"));

	if (filename.isEmpty())
		return;

	auto hpgl = renderToHPGL2(s->sortedShowPaths, s->mediaSize.width(), s->mediaSize.height());

	QSaveFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(this, "Error writing HPGL2", "Couldn't open file");
		return;
	}

	file.write(hpgl.data(), static_cast<qint64>(hpgl.size()));
	if (!file.commit())
	{
		QMessageBox::critical(this, "Error writing HPGL2", "Couldn't write data");
		return;
	}
}
