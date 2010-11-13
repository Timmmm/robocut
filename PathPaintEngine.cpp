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
	if(!isCosmetic)
	{
		QList<QPolygonF> polys = path.toSubpathPolygons();
		for (int i = 0; i < polys.size(); ++i)
		{
			if(dashPattern.empty()) dev->addPath(transform.map(polys[i]), dashPattern);
			else
			{
				QPolygonF polytemp = transform.map(polys[i]);
				QPolygonF newpoly;
				for (int j = 0; j < polytemp.size(); ++j)
				{					
					newpoly.append(QPointF ( polytemp[j].x(), polytemp[j].y() ));
				}
				dev->addPath(newpoly, dashPattern);
			}
		}
	}
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
	dev->addPath(transform.map(p), dashPattern);
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
	dashPattern = state.pen().dashPattern();
	isCosmetic  = state.pen().isCosmetic(); 
}
