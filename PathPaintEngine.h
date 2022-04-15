#pragma once

#include <QPaintEngine>
#include <QPaintDevice>
#include <QPainterPath>
#include "PathPaintDevice.h"

// This is a simple paint engine that records all the paths painted.
// It is used with QSvgPainter to record the paths in an SVG.
// The only device it can draw to is PathPaintDevice
class PathPaintEngine : public QPaintEngine
{
public:
	PathPaintEngine();
	
	bool begin(QPaintDevice* pdev) override;
	bool end() override;

	void drawPath(const QPainterPath& path) override;
	void drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr) override;
	void drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode) override;

	Type type() const override;
	
	void updateState(const QPaintEngineState& s) override;
	
private:
	PathPaintDevice* dev = nullptr;
	QTransform transform;
	QVector<qreal> dashPattern;
	bool zeroWidthPen = false;
};

