#include "PlotterPage.h"

#include "PathPaintEngine.h"

#include <QDebug>

PlotterPage::PlotterPage(double widthInMm, double heightInMm)
{
	engine = NULL;
	width = widthInMm;
	height = heightInMm;
}

PlotterPage::~PlotterPage()
{
	if (engine)
		delete engine;
}

void PlotterPage::addPath(QPolygonF& path)
{
	pagePaths.append(path);
}

QPaintEngine* PlotterPage::paintEngine() const
{
	if (!engine)
		engine = new PathPaintEngine(const_cast<PlotterPage*>(this));
	return engine;
}

QList<QPolygonF> PlotterPage::paths()
{
	return pagePaths;
}

int PlotterPage::metric(PaintDeviceMetric metric) const
{
	const double ppm = 100.0; // Points per mm.
	switch (metric)
	{
	case PdmWidth:
		return width*ppm;
	case PdmHeight:
		return height*ppm;
	case PdmWidthMM:
		return width;
	case PdmHeightMM:
		return height;
	case PdmNumColors:
		return 2;
	case PdmDepth:
		return 1;
	case PdmDpiX:
		return ppm * 25.4; // Convert to inches.
	case PdmDpiY:
		return ppm * 25.4;
	case PdmPhysicalDpiX:
		return ppm * 25.4;
	case PdmPhysicalDpiY:
		return ppm * 25.4;
	}
	return 0;
}
