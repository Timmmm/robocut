#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CuttingDialog.h"
#include "PathPaintDevice.h"
#include "PathSorter.h"
#include "Plotter.h"
#include "SvgRenderer.h"

#include <cmath>
#include <iostream>

#include <QDebug>
#include <QDir>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QSettings>
#include <QShortcut>
#include <QSvgRenderer>
#include <QWheelEvent>

#include <algorithm>

using namespace std;

namespace
{
	bool fileDoesntExist(const QString& filename)
	{
		return !QFile::exists(filename);
	}

	void removeNonexistentFiles(QStringList& filenames)
	{
		filenames.erase(
			std::remove_if(filenames.begin(), filenames.end(), fileDoesntExist),
			filenames.end()
		);
	}

}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
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
	
	ui->actionGrid->setChecked(gridEnabled);
	ui->actionDimensions->setChecked(dimensionsEnabled);
	ui->actionRulers->setChecked(rulersEnabled);
	ui->actionCutter_Path->setChecked(cutterPathEnabled);
	
	// TODO: Implement this.
	ui->menuEdit->hide();

	scene = new QGraphicsScene(this);
	
	ui->graphicsView->setScene(scene);
	ui->graphicsView->scale(defaultZoom, defaultZoom);
	ui->graphicsView->viewport()->installEventFilter(this);

	animationTimer = new QTimer(this);
	connect(animationTimer, &QTimer::timeout, this, &MainWindow::animate);
	
	removeNonexistentFiles(recentFiles);
	recentFilesModel = new SvgPreviewModel(64, this);
	recentFilesModel->setFiles(recentFiles);
	ui->recentFilesList->setModel(recentFilesModel);
	
	exampleFilesModel = new SvgPreviewModel(64, this);
	
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
	
	auto sortMethodGroup = new QActionGroup(this);
	sortMethodGroup->addAction(ui->actionSortShortest);
	sortMethodGroup->addAction(ui->actionSortByY);
	sortMethodGroup->addAction(ui->actionSortGreedy);
	sortMethodGroup->addAction(ui->actionSortInsideFirst);
	sortMethodGroup->addAction(ui->actionSortNone);
	
	connect(sortMethodGroup, &QActionGroup::triggered, this, &MainWindow::onSortMethodTriggered);

	show();	
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
	if (!QFile::exists(currentFilename))
	{
		qDebug() << "Reload failed. File missing: " << currentFilename;
		return;
	}
	
	loadFile(currentFilename);
}

void MainWindow::on_actionIdentify_triggered()
{
	// CAUTION: only call this when not cutting!
	CutterId id = DetectDevices();
	cout << "Identify:";
	if (id.usb_product_id)
	{
		cout << " usb=" << id.usb_vendor_id << "/" << id.usb_product_id;
	}
	cout << " " << id.msg << endl;
}

void MainWindow::loadFile(QString filename)
{
	qDebug() << "Reading file: " << filename;
	
	// Convert the SVG to paths.
	auto render = svgToPaths(filename, true);

	if (!render.success)
	{
		QMessageBox::critical(this, "Error loading file.", "Couldn't open the file for reading.");
		qDebug() << "Error loading svg.";
		return;
	}
	
	if (render.hasTspanPosition)
	{
		QMessageBox::warning(this, "Multi-line Text", "This SVG contains multi-line text which "
		                     "is not supported in SVG Tiny. Text may be rendered incorrectly. "
		                     "The easiest workaround is to convert the text to paths.");
	}
	
	// TODO: Use the width and height attributes.
		
	// See https://code.woboq.org/qt5/qtsvg/src/svg/qsvghandler.cpp.html#_ZL15convertToPixelsdbN11QSvgHandler10LengthTypeE
	// The default size is derived from the width="" height="" svg attribute tags
	// assuming 90 DPI
	mediaSize = render.viewBox.size();
	
	QRectF pageRect(0.0, 0.0, mediaSize.width(), mediaSize.height());
		
	auto viewSize = ui->centralWidget->size();
	
	defaultZoom = 0.8 * std::min(viewSize.width() / mediaSize.width(),
	                             viewSize.height() / mediaSize.height());

	QPointF startingPoint(0.0, mediaSize.height());

	// Sort the paths with the following goals:
	//
	// 1. Minimise the time spend travelling without cutting.
	// 2. Try to cut inside shapes before outside shapes.
	// 3. Try not to travel too much in the Y direction (it can lead to accumulation of
	//    errors due to the vinyl slipping in the rollers).
	auto sortedPaths = sortPaths(render.paths,
	                             sortMethod,
	                             startingPoint);
	
	// Save the paths for the animation.
	paths = sortedPaths;
	
	// Clear the scene.
	scene->clear();
	// And reset the sceneRect, which it doesn't do by default.
	scene->setSceneRect(pageRect.adjusted(-20, -20, 20, 20));
	
	scene->setBackgroundBrush(QBrush(Qt::lightGray));
	

	// Add the page background
	scene->addRect(pageRect, Qt::NoPen, QBrush(Qt::white));
	
	// Add the dimensions test.
	dimensionsItem = scene->addText(QString::number(mediaSize.width()) + " × " +
	                                QString::number(mediaSize.height()) + " mm",
	                                QFont("Helvetica", 10));
	dimensionsItem->setPos(0.0, mediaSize.height());
	dimensionsItem->setVisible(dimensionsEnabled);
	
	// Add the rulers.
//	addRulers(scene);
	
	QList<QGraphicsItem*> gridSquares;
	
	// Add the centimetre grid.
	for (int x = 0; x < mediaSize.width() / 10; ++x)
	{
		for (int y = x % 2; y < mediaSize.height() / 10; y += 2)
		{
			int w = std::min(10.0, mediaSize.width() - x * 10.0);
			int h = std::min(10.0, mediaSize.height() - y * 10.0);
			gridSquares.append(
			            scene->addRect(x*10, mediaSize.height() - y*10 - h, w, h,
						               Qt::NoPen, QBrush(QColor::fromRgb(240, 240, 240)))
			        );
		}
	}
	
	gridItem = scene->createItemGroup(gridSquares);
	gridItem->setVisible(gridEnabled);	
	
	// Add the outline of the page.
	QPen pageOutlinePen;
	pageOutlinePen.setCosmetic(true);
	scene->addRect(pageRect, pageOutlinePen, Qt::NoBrush);
	        
	// Add all the paths.
	double hue = 0.0;
	const double golden = 0.5 * (1.0 + sqrt(5.0));
	
	for (const auto& path : paths)
	{
		QPen pen(QColor::fromHsvF(hue, 1.0, 0.7));
		
		// Don't change the pen width with zoom.
		pen.setCosmetic(true);
		pen.setWidthF(3.0);
		
		// Add the path to the painter path, but offset to account for the viewBox position.
		QPainterPath pp;
        pp.addPolygon(path.translated(-render.viewBox.top(), -render.viewBox.left()));
		// addPath() is used rather than addPolygon() because addPolygon() is always closed.
        scene->addPath(pp, pen);
				
		hue += golden;
		hue -= trunc(hue);
	}
	
	// Add the lines that show where the cutter goes between shapes in light grey.
	QList<QGraphicsItem*> cutterPathLines;
	
	QPointF currentPoint = startingPoint;
	QPen pen(QColor::fromHsvF(0.0, 0.0, 0.8));
	// Don't change the pen width with zoom.
	pen.setCosmetic(true);
	pen.setWidthF(3.0);
	for (const auto& path : paths)
	{
		cutterPathLines.append(scene->addLine(QLineF(currentPoint, path.first()), pen));
		currentPoint = path.last();
	}
	
	cutterPathItem = scene->createItemGroup(cutterPathLines);
	cutterPathItem->setVisible(cutterPathEnabled);	

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
	
	// TODO: What do I want to do here?
//	if (clipped)
//		QMessageBox::warning(this, "Paths clipped", "<b>WARNING!</b><br><br>Some paths lay outside the 210&times;297&thinsp;mm A4 area. These have been squeezed back onto the page in a most ugly fashion, so cutting will almost certainly not do what you want.");

	// Change window title and enable menu items.
	setFileLoaded(filename);
}

void MainWindow::on_actionAbout_triggered()
{
	QString message = QString("<b>") + ROBOCUT_VERSION +
	"</b><br><br>By Tim Hutt, &copy; 2010<br/>" +
	"<br>Parts of the source by Markus Schulz, &copy; 2010<br/>" +
	"<br/>This software allows you to read a vector image in <a href=\"http://en.wikipedia.org/wiki/Scalable_Vector_Graphics\">SVG format</a>, " +
	"and send it to a <a href=\"http://www.graphteccorp.com/craftrobo/\">Graphtec Craft Robo 2</a>/3 " +
	" or <a href=\"http://www.silhouette.com/cameo\">Silhouette Cameo</a> " +
	" (or similar device) for cutting. It is designed to work with SVGs produced " +
	"by the excellent free vector graphics editor <a href=\"http://www.inkscape.org/\">Inkscape</a>. " +
	" It may work with other software but this has not been tested.<br/>" +
	"<br/>See <a href=\"http://robocut.org\">the website for more information</a>.";
	QMessageBox::information(this, "About", message);
}

void MainWindow::on_actionExit_triggered()
{
	close();
}

void MainWindow::on_actionCut_triggered()
{
	if (!cutDialog)
		cutDialog = new CutDialog(this);

	if (cutDialog->exec() != QDialog::Accepted)
		return;

	// Create a new dialog and run the actual cutting in a different thread.

	CuttingDialog* cuttingDlg = new CuttingDialog(this);

	CutParams params;
	params.cuts = paths;
	params.mediawidth = mediaSize.width();
	params.mediaheight = mediaSize.height();
	params.media = cutDialog->media();
	params.pressure = cutDialog->pressure();
	params.regwidth = cutDialog->regWidth();
	params.regheight = cutDialog->regHeight();
	params.regmark = cutDialog->regMark();
	params.regsearch = cutDialog->regSearch();
	params.speed = cutDialog->speed();
	params.trackenhancing = cutDialog->trackEnhancing();

	cuttingDlg->startCut(params);
	cuttingDlg->show();
}

void MainWindow::on_actionManual_triggered()
{
	QMessageBox::information(this, "Manual", "An online manual is available at <br><br><a href=\"http://robocut.org/\">http://robocut.org/</a>");
}

void MainWindow::on_actionAnimate_toggled(bool animate)
{
	if (animate)
	{
		animationTimer->start(30);
		if (cutMarker)
		{
			cutMarker->setPos(0, 0);
			cutMarker->show();
		}

		cutMarkerPoly = 0;
		cutMarkerLine = 0;
		cutMarkerDistance = 0.0;
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
	ui->graphicsView->resetTransform();
	ui->graphicsView->scale(defaultZoom, defaultZoom);
}

void MainWindow::on_actionZoom_In_triggered()
{
	ui->graphicsView->scale(1.2, 1.2);
}

void MainWindow::on_actionZoom_Out_triggered()
{
	ui->graphicsView->scale(1.0/1.2, 1.0/1.2);
}

void MainWindow::animate()
{
	// TODO: This method is a bit of a mess - could do with moving the
	// iteration stuff into a separate iterator class.
	
	if (!cutMarker)
		return;

	// Make sure the current position is sane.
	if (cutMarkerPoly >= paths.size()*2+1)
	{
		// If not, reset it.
		cutMarkerPoly = 0;
		cutMarkerLine = 0;
		cutMarkerDistance = 0.0;
		return;
	}
	// Get the ends of the segment/edge we are currently on.

	QPointF startingPoint(0.0, mediaSize.height());
	
	// How far to travel each "frame". This controls the animation speed.
	double distanceRemaining = 2.0;
	
	while (true)
	{
		// Start and end points of the current line.
		QPointF a;
		QPointF b;
		
		if (cutMarkerPoly % 2 == 0)
		{
			// We are moving between paths, and not cutting.
			if (cutMarkerPoly == 0)
				a = startingPoint;
			else
				a = paths[(cutMarkerPoly / 2)-1].back();
			
			if (cutMarkerPoly >= paths.size()*2)
				b = startingPoint;
			else
				b = paths[(cutMarkerPoly / 2)].front();

			cutMarker->setOpacity(0.2);
		}
		else
		{
			auto pathIndex = (cutMarkerPoly - 1) / 2;
			a = paths[pathIndex][cutMarkerLine];
			b = paths[pathIndex][cutMarkerLine+1];
			cutMarker->setOpacity(1.0);			
		}
		
		QLineF ln(a, b);
		
		// If the cutter shouldn't be in this line, go to the next one.
		if ((ln.length() - cutMarkerDistance) < distanceRemaining)
		{
			distanceRemaining -= (ln.length() - cutMarkerDistance);
			cutMarkerDistance = 0.0;
			if (cutMarkerPoly % 2 != 0 && cutMarkerLine < paths[(cutMarkerPoly - 1) / 2].size()-2)
			{
				++cutMarkerLine;
			}
			else
			{
				cutMarkerPoly = (cutMarkerPoly + 1) % (paths.size() * 2 + 1);
				cutMarkerLine = 0;
			}
			continue;
		}
		
		cutMarkerDistance += distanceRemaining;
		
		// Get a vector along the current line.
		QPointF r = b-a;
		double h = hypot(r.x(), r.y());
		if (h > 0.0)
			r /= h;
		
		// The current position of the marker.
		QPointF p = a + r * cutMarkerDistance;
	
		cutMarker->setPos(p);
		break;
	}
}
void MainWindow::setFileLoaded(QString filename)
{
	bool e = !filename.isEmpty();
	if (e)
		setWindowTitle("Robocut - " + filename);
	else
		setWindowTitle("Robocut");

	ui->actionAnimate->setEnabled(e);
	ui->actionReset->setEnabled(e);
	ui->actionZoom_In->setEnabled(e);
	ui->actionZoom_Out->setEnabled(e);
	ui->actionCut->setEnabled(e);
	ui->actionReload->setEnabled(e);
	ui->actionClose->setEnabled(e);
	
	// Update the recent files list. Also ignore files that start
	// with a colon - those are examples.
	if (!filename.isEmpty() && !filename.startsWith(":"))
	{
		qDebug() << "Adding" << filename << "to recents";
		recentFiles.removeAll(filename);
		recentFiles.prepend(filename);
	}
	
	currentFilename = filename;
}

void MainWindow::on_openSvgButton_clicked()
{
	on_actionOpen_triggered();
}

void MainWindow::on_actionClose_triggered()
{
	setFileLoaded("");
	scene->clear();
	ui->stackedWidget->setCurrentIndex(0);
	// Update the recent files - a new one may have been loaded. Unfortunately
	// this re-renders them all, but eh.
	// TODO: Don't re-render them all.
	recentFilesModel->setFiles(recentFiles);
}

void MainWindow::on_recentFilesList_activated(const QModelIndex &index)
{	
	auto filename = index.data(SvgPreviewModel::FilenameRole).toString();
	
	lastOpenDir = QFileInfo(filename).absoluteDir().path();
	loadFile(filename);
}

void MainWindow::on_examplesList_activated(const QModelIndex &index)
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
	
	// TODO: This resets the view. It would be nice if it didn't need to actually reload the
	// whole file and we just resorted the polygons.
	on_actionReload_triggered();
}


void MainWindow::on_actionRulers_toggled(bool enabled)
{
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

void MainWindow::on_actionVerify_Adjust_Scale_triggered()
{
    // TODO: Allow the user to select two vertices (snap-to-vertex) and
	// then a dialog will open saying the distance between those points and
	// giving the option to specify what the distance *should* be, thereby
	// scaling the entire thing.
}

void MainWindow::on_actionCutter_Path_toggled(bool enabled)
{
	cutterPathEnabled = enabled;
	if (cutterPathItem != nullptr)
		cutterPathItem->setVisible(cutterPathEnabled);
    
}
