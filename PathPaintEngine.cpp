#include "PathPaintEngine.h"

#include <QDebug>

PathPaintEngine::PathPaintEngine(QPaintDevice* pdev)
	: QPaintEngine(AllFeatures)
{
	dev = dynamic_cast<PathPaintDevice*>(pdev);
	if (!dev)
		qWarning("PathPaintEngine: unsupported target device.");
}

bool PathPaintEngine::begin(QPaintDevice* pdev)
{
	dev = dynamic_cast<PathPaintDevice*>(pdev);
	if (!dev)
		qWarning("PathPaintEngine: unsupported target device.");

	// TODO: setActive?
	return true;
}

void PathPaintEngine::drawPath(const QPainterPath& path)
{
	if (!dev)
		return;

	QList<QPolygonF> polys = path.toSubpathPolygons();
	for (int i = 0; i < polys.size(); ++i)
		dev->addPath(transform.map(polys[i]));
}

void PathPaintEngine::drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr)
{
	// Nop.
}

void PathPaintEngine::drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode)
{
	if (!dev)
		return;

	QPolygonF p;
	for (int i = 0; i < pointCount; ++i)
		p.append(points[i]);
	dev->addPath(transform.map(p));
}

bool PathPaintEngine::end()
{
	if (!dev)
		return false;
	return true;
}

QPaintEngine::Type PathPaintEngine::type() const
{
	return QPaintEngine::User;
}
void PathPaintEngine::updateState(const QPaintEngineState& state)
{
	if (state.state() & DirtyTransform)
		transform = state.transform();
}
