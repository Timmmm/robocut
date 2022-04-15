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
		return static_cast<int>(width * ppm);
	case PdmHeight:
		return static_cast<int>(height * ppm);
	case PdmWidthMM:
		return static_cast<int>(width);
	case PdmHeightMM:
		return static_cast<int>(height);
	case PdmNumColors:
		return 2;
	case PdmDepth:
		return 1;
	case PdmDpiX:
		return static_cast<int>(25.4 * ppm); // Convert to inches.
	case PdmDpiY:
		return static_cast<int>(25.4 * ppm);
	case PdmPhysicalDpiX:
		return static_cast<int>(25.4 * ppm);
	case PdmPhysicalDpiY:
		return static_cast<int>(25.4 * ppm);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
	case PdmDevicePixelRatio:
		return 1;
	case PdmDevicePixelRatioScaled:
		return 1;
#endif
	}
	return 0;
}

