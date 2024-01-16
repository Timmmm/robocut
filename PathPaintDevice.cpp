#include "PathPaintDevice.h"

#include "PathPaintEngine.h"

#include <QDebug>

size_t qHash(const QPolygonF& key)
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

PathPaintDevice::PathPaintDevice(double width, double height)
	: width(width), height(height)
{
}

PathPaintDevice::~PathPaintDevice()
{
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
		engine = std::make_unique<PathPaintEngine>();
	return engine.get();
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
		return static_cast<int>(width);
	case PdmHeight:
		return static_cast<int>(height);
	case PdmWidthMM:
		return static_cast<int>(width);
	case PdmHeightMM:
		return static_cast<int>(height);
	case PdmNumColors:
		return 2;
	case PdmDepth:
		return 1;
	case PdmDpiX:
		return static_cast<int>(25.4);
	case PdmDpiY:
		return static_cast<int>(25.4);
	case PdmPhysicalDpiX:
		return static_cast<int>(25.4);
	case PdmPhysicalDpiY:
		return static_cast<int>(25.4);
	case PdmDevicePixelRatio:
		return 1;
	case PdmDevicePixelRatioScaled:
		return 1;
	}
	return 0;
}
