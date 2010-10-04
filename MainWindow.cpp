#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "PlotterPage.h"
#include "Plotter.h"
#include <QPainter>
#include <QDebug>

#include <QSvgRenderer>

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	scene = new QGraphicsScene(this);
	ui->graphicsView->setScene(scene);

	ui->graphicsView->scale(2.0, 2.0);

	cutDialog = NULL;
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
	QString filename = QFileDialog::getOpenFileName(this,
			tr("Open File"), QDir::homePath(), tr("SVG Files (*.svg)"));
	if (filename.isEmpty())
		return;

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
		}

		pen.setColor(QColor(rand() % 155, rand() % 155, rand() % 155));
		scene->addPolygon(newPaths[i], pen);

		paths.append(newPaths[i]);
	}

	qDebug() << "Paths: " << addedPaths;



	update();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information(this, "About", "Graphtec Craft Robo 2 software by Tim Hutt.");
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

	// TODO: Move to another thread with a cancel dialog.
	Error e = Cut(paths, cutDialog->media(), cutDialog->speed(), cutDialog->pressure(), cutDialog->trackEnhancing());
	if (!e)
		qDebug() << e.message().c_str();
}

void MainWindow::on_actionPreview_triggered()
{
	// TODO: Show an animation of how the cutting will happen.
	QMessageBox::information(this, "TODO", "In future this will show an animation of the cutting process.");
}

void MainWindow::on_actionManual_triggered()
{
	QMessageBox::information(this, "TODO", "Of course...");
}
