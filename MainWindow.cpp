#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "PlotterPage.h"
#include <QPainter>
#include <QDebug>

#include <QSvgRenderer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	scene = new QGraphicsScene(this);
	ui->graphicsView->setScene(scene);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
	QString filename = "/home/thutt/palm.svg";
	QSvgRenderer rend;
	if (!rend.load(filename))
		qDebug() << "Error loading svg.";

	PlotterPage pg(210.0, 297.0);
	QPainter p(&pg);

	rend.render(&p);

	paths = pg.paths();

	QPen pen;
	pen.setWidthF(0.5);
	for (int i = 0; i < paths.size(); ++i)
		scene->addPolygon(paths[i], pen);

	update();
}
