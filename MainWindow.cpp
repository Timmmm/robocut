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

	PlotterPage pg(100.0, 100.0);
	QPainter p(&pg);

	rend.render(&p);

}
