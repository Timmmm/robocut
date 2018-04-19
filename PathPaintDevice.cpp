#include "PathPaintDevice.h"

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
	engine = nullptr;
	width = widthInMm;
	height = heightInMm;
	ppm = pixelsPerMm;
	if (ppm == 0.0)
		ppm = 1.0;
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
}

QPaintEngine* PathPaintDevice::paintEngine() const
{
	if (!engine)
		engine = new PathPaintEngine();
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
	case PdmDevicePixelRatio:
		return 1;
	case PdmDevicePixelRatioScaled:
		return 1;
#endif
	}
	return 0;
}

