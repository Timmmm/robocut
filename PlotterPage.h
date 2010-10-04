#pragma once

#include <QPaintDevice>
#include <QList>
#include <QPolygonF>

class PathPaintEngine;

class PlotterPage : public QPaintDevice
{
public:
	PlotterPage(double widthInMm, double heightInMm);
	~PlotterPage();

	void addPath(QPolygonF& path);

	QPaintEngine* paintEngine() const;

protected:
	int metric(PaintDeviceMetric metric) const;
private:
	mutable PathPaintEngine* engine;
	QList<QPolygonF> paths;

	double width;
	double height;
};
