#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "PlotterPage.h"
#include "Plotter.h"

#include "CuttingDialog.h"
#include <QPainter>
#include <QDebug>

#include <QSvgRenderer>

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

#include <QShortcut>
#include <cmath>

using namespace std;

const double InitialZoom = 2.0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	scene = new QGraphicsScene(this);
	ui->graphicsView->setScene(scene);

	ui->graphicsView->scale(InitialZoom, InitialZoom);

	cutDialog = NULL;

	animationTimer = new QTimer(this);
	connect(animationTimer, SIGNAL(timeout()), SLOT(animate()));
	cutMarker = NULL;

	// Alternative zoom shortcuts
	QShortcut* zoom_in = new QShortcut(QKeySequence("X"), this);
	connect(zoom_in, SIGNAL(activated()), SLOT(on_actionZoom_In_triggered()));
	QShortcut* zoom_out = new QShortcut(QKeySequence("Z"), this);
	connect(zoom_out, SIGNAL(activated()), SLOT(on_actionZoom_Out_triggered()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

uint qHash(const QPolygonF& key)
{
	QByteArray repr;
	for (int i = 0; i < key.size(); ++i)
	{
		double x = key[i].x();
		double y = key[i].y();

		repr.append(reinterpret_cast<char*>(&x), sizeof(x));
		repr.append(reinterpret_cast<char*>(&y), sizeof(y));
	}
	return qHash(repr);
}

void MainWindow::on_actionOpen_triggered()
{
	if (lastOpenDir.isEmpty())
		lastOpenDir = QDir::homePath();

	QString filename = QFileDialog::getOpenFileName(this,
			tr("Open File"), lastOpenDir, tr("SVG Files (*.svg)"));
	if (filename.isEmpty())
		return;

	lastOpenDir = QFileInfo(filename).absoluteDir().path();

	qDebug() << "lastOpenDir: " << lastOpenDir;

	qDebug() << "Reading file: " << filename;

	QSvgRenderer rend;
	if (!rend.load(filename))
	{
		QMessageBox::critical(this, "Error loading file.", "Couldn't open the file for reading.");
		qDebug() << "Error loading svg.";
		return;
	}

	PlotterPage pg(210.0, 297.0);
	QPainter p(&pg);

	rend.render(&p);

	// TODO: Remove duplicate paths.
	QList<QPolygonF> newPaths = pg.paths();

	paths.clear();

	QSet<QPolygonF> addedAlready;

	scene->clear();
	scene->setBackgroundBrush(QBrush(Qt::lightGray));

	// The page.
	scene->addRect(0.0, 0.0, 210.0, 297.0, QPen(), QBrush(Qt::white));

	bool pathsClipped = false;
	int addedPaths = 0;
	QPen pen;
	pen.setWidthF(0.5);
	for (int i = 0; i < newPaths.size(); ++i)
	{
		// Need to remove duplicates because the SVG paint engine draws these twice
		// for some reason...
		if (addedAlready.contains(newPaths[i]))
			continue;

		addedAlready.insert(newPaths[i]);
		++addedPaths;

		for (int j = 0; j < newPaths[i].size(); ++j)
		{
			// Because inkscape is retarded and *always* exports SVG in units of px, with a default
			// resolution of 90 dpi.
			newPaths[i][j] /= 90.0/25.4;
			if (newPaths[i][j].x() < 0.0)
			{
				pathsClipped = true;
				newPaths[i][j].setX(0.0);
			}
			if (newPaths[i][j].y() < 0.0)
			{
				pathsClipped = true;
				newPaths[i][j].setY(0.0);
			}
			if (newPaths[i][j].x() > 210.0)
			{
				pathsClipped = true;
				newPaths[i][j].setX(210.0);
			}
			if (newPaths[i][j].y() > 297.0)
			{
				pathsClipped = true;
				newPaths[i][j].setY(297.0);
			}
		}

		pen.setColor(QColor(rand() % 155, rand() % 155, rand() % 155));
		scene->addPolygon(newPaths[i], pen);

		paths.append(newPaths[i]);
	}

	// Handle the animation. I.e. stop it.
	// The old one was deleted when we cleared the scene.
	cutMarker = scene->addEllipse(-1.0, -1.0, 2.0, 2.0, QPen(Qt::black), QBrush(Qt::red));
	ui->actionAnimate->setChecked(false);

	qDebug() << "Paths: " << addedPaths;
	on_actionReset_triggered();
	update();

	if (pathsClipped)
		QMessageBox::warning(this, "Paths clipped", "<b>BIG FAT WARNING!</b><br><br>Some paths lay outside the 210&times;297&thinsp;mm A4 area. These have been squeezed back onto the page in a most ugly fashion, so cutting will almost certainly not do what you want.");
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information(this, "About", "<b>Robocut 0.1</b><br><br>By Tim Hutt, &copy; 2010<br/><br/>This software allows you to read a vector image in <a href=\"http://en.wikipedia.org/wiki/Scalable_Vector_Graphics\">SVG format</a>, and send it to a <a href=\"http://www.graphteccorp.com/craftrobo/\">Graphtec Craft Robo 2</a> (or possibly 3) for cutting. It is designed to work with SVGs produced by the excellent free vector graphics editor <a href=\"http://www.inkscape.org/\">Inkscape</a>. It may work with other software but this has not been tested.<br/><br/>See <a href=\"http://concentriclivers.com/\">the online manual for instructions</a>.");
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

//	CuttingDialog* cuttingDlg = new CuttingDialog(this);
//	cuttingDlg->show();// TODO:


	Error e = Cut(paths, cutDialog->media(), cutDialog->speed(), cutDialog->pressure(), cutDialog->trackEnhancing());
	if (!e)
		qDebug() << e.message().c_str();
}

void MainWindow::on_actionManual_triggered()
{
	QMessageBox::information(this, "Manual", "An online manual is available at <br><br><a href=\"http://concentriclivers.com/\">http://concentriclivers.com/</a>");
}

void MainWindow::on_actionAnimate_toggled(bool animate)
{
	if (animate)
	{
		animationTimer->start(50);
		if (cutMarker)
		{
			cutMarker->setPos(0, 0);
			cutMarker->setVisible(true);
		}

		cutMarkerPath = 0;
		cutMarkerLine = 0;
		cutMarkerDistance = 0.0;
	}
	else
	{
		animationTimer->stop();
		if (cutMarker)
			cutMarker->setVisible(false);
	}
}

void MainWindow::on_actionReset_triggered()
{
	ui->graphicsView->resetTransform();
	ui->graphicsView->scale(InitialZoom, InitialZoom);
}

void MainWindow::on_actionZoom_In_triggered()
{
	ui->graphicsView->scale(1.2, 1.2);
}

void MainWindow::on_actionZoom_Out_triggered()
{
	ui->graphicsView->scale(0.8, 0.8);
}

void MainWindow::animate()
{
	if (!cutMarker)
		return;

	// Make sure the current position is sane.
	if (cutMarkerPath >= paths.size() || cutMarkerLine >= paths[cutMarkerPath].size())
	{
		// If not, reset it.
		cutMarkerPath = 0;
		cutMarkerLine = 0;
		cutMarkerDistance = 0.0;
		return;
	}
	// Get the ends of the segment/edge we are currently on.

	QPointF a = paths[cutMarkerPath][cutMarkerLine];
	QPointF b;
	if (cutMarkerLine == paths[cutMarkerPath].size()-1)
	{
		// We are moving between paths, and not cutting.
		b = paths[(cutMarkerPath+1) % paths.size()][0];
		cutMarker->setOpacity(0.3);
	}
	else
	{
		// We are cutting on a path.
		b = paths[cutMarkerPath][cutMarkerLine+1];
		cutMarker->setOpacity(1.0);
	}

	QPointF r = b-a;
	double h = hypot(r.x(), r.y());
	if (h > 0.0)
		r /= h;

	// The current position of the marker.
	QPointF p = a + r * cutMarkerDistance;

	cutMarker->setPos(p);

	// Advance the position of the marker for the next time this function is called.
	cutMarkerDistance += 2.0;
	if (cutMarkerDistance > h)
	{
		cutMarkerDistance = 0.0;
		if (++cutMarkerLine >= paths[cutMarkerPath].size())
		{
			cutMarkerLine = 0;
			if (++cutMarkerPath >= paths.size())
			{
				cutMarkerPath = 0;

				// Also stop the animation... maybe?
				//ui->actionAnimate->setChecked(false);
			}
		}
	}
}
