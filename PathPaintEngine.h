#pragma once

#include <QPaintEngine>
#include <QPaintDevice>
#include <QPainterPath>
#include "PathPaintPage.h"

// This is a simple paint engine that records all the paths painted.
// It is used with QSvgPainter to record the paths in an SVG.
// The only device it can draw to is PathPaintDevice
class PathPaintEngine : public QPaintEngine
{
public:
	PathPaintEngine(QPaintDevice* pdev);
	bool begin(QPaintDevice* pdev);

	void drawPath(const QPainterPath& path);

	void drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr);

	void drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode);

	bool end();

	Type type() const;
	void updateState(const QPaintEngineState& state);
private:
	PathPaintDevice* dev;
	QTransform transform;
};

