#pragma once

#include <QPaintEngine>
#include <QPaintDevice>
#include <QPainterPath>
#include "PlotterPage.h"

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
	PlotterPage* dev;
};

