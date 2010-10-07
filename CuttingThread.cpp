#include "CuttingThread.h"

#include "Plotter.h"

CuttingThread::CuttingThread(QObject *parent) :
    QThread(parent)
{
	_media = 300;
	_speed = 10;
	_pressure = 10;
	_trackenhancing = false;
}


void CuttingThread::setParams(QList<QPolygonF> cuts, int media, int speed, int pressure, bool trackenhancing)
{
	_cuts = cuts;
	_media = media;
	_speed = speed;
	_pressure = pressure;
	_trackenhancing = trackenhancing;
}


void CuttingThread::run()
{
	Error e = Cut(_cuts, _media, _speed, _pressure, _trackenhancing);
	if (e)
		emit success();
	else
		emit error(e.message().c_str());
	exec();
}
