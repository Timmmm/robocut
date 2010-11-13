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



PathPaintDevice::PathPaintDevice(double widthInMm, double heightInMm, double pixelsPerMm)
{
	engine = NULL;
	width = widthInMm;
	height = heightInMm;
	pathsClipped = false;
	ppm = pixelsPerMm;
	if (ppm == 0.0)
		ppm = 1.0;
}

PathPaintDevice::~PathPaintDevice()
{
	if (engine)
		delete engine;
}

void PathPaintDevice::addPath(const QPolygonF& path, const QVector<qreal> dashpattern)
{
	if (pagePathSet.contains(path))
		return;
	if(!dashpattern.empty()) qWarning("DASH\n");
	//pagePathSet.insert(path);
	pagePaths.append(path);

	// Clip the path.
	for (int j = 0; j < pagePaths.back().size(); ++j)
	{
		// pagePaths are in mm, so convert from pixels to mm.
		pagePaths.back()[j] /= ppm;

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
	// Width in pixels.
	case PdmWidth:
		return width * ppm;
	case PdmHeight:
		return height * ppm;
	case PdmWidthMM:
		return width;
	case PdmHeightMM:
		return height;
	case PdmNumColors:
		return 2;
	case PdmDepth:
		return 1;
	case PdmDpiX:
		return 25.4 * ppm; // Convert to inches.
	case PdmDpiY:
		return 25.4 * ppm;
	case PdmPhysicalDpiX:
		return 25.4 * ppm;
	case PdmPhysicalDpiY:
		return 25.4 * ppm;
	}
	return 0;
}

bool PathPaintDevice::clipped() const
{
	return pathsClipped;
}
