#include "PathPaintPage.h"

#include "PathPaintEngine.h"

#include <QDebug>



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



PathPaintDevice::PathPaintDevice(double widthInMm, double heightInMm, double pixelsPerInch)
{
	engine = NULL;
	width = widthInMm;
	height = heightInMm;
	pathsClipped = false;
	ppi = pixelsPerInch;
	if (ppi == 0.0)
		ppi = 1.0;
}

PathPaintDevice::~PathPaintDevice()
{
	if (engine)
		delete engine;
}

void PathPaintDevice::addPath(const QPolygonF& path)
{
	if (pagePathSet.contains(path))
		return;
	pagePathSet.insert(path);
	pagePaths.append(path);

	// Clip the path.
	for (int j = 0; j < pagePaths.back().size(); ++j)
	{
		// Because inkscape is retarded and *always* exports SVG in units of px, with a default
		// resolution of 90 dpi.

		pagePaths.back()[j] /= (ppi / 25.4);

		if (pagePaths.back()[j].x() < 0.0)
		{
			pathsClipped = true;
			pagePaths.back()[j].setX(0.0);
		}
		if (pagePaths.back()[j].y() < 0.0)
		{
			pathsClipped = true;
			pagePaths.back()[j].setY(0.0);
		}
		if (pagePaths.back()[j].x() > width)
		{
			pathsClipped = true;
			pagePaths.back()[j].setX(width);
		}
		if (pagePaths.back()[j].y() > height)
		{
			pathsClipped = true;
			pagePaths.back()[j].setY(height);
		}
	}

}

QPaintEngine* PathPaintDevice::paintEngine() const
{
	if (!engine)
		engine = new PathPaintEngine(const_cast<PathPaintDevice*>(this));
	return engine;
}

QList<QPolygonF> PathPaintDevice::paths()
{
	return pagePaths;
}

int PathPaintDevice::metric(PaintDeviceMetric metric) const
{
	switch (metric)
	{
	case PdmWidth:
		return width;
	case PdmHeight:
		return height;
	case PdmWidthMM:
		return width;
	case PdmHeightMM:
		return height;
	case PdmNumColors:
		return 2;
	case PdmDepth:
		return 1;
	case PdmDpiX:
		return 25.4; // Convert to inches.
	case PdmDpiY:
		return 25.4;
	case PdmPhysicalDpiX:
		return 25.4;
	case PdmPhysicalDpiY:
		return 25.4;
	}
	return 0;
}

bool PathPaintDevice::clipped() const
{
	return pathsClipped;
}
