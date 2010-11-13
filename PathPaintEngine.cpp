#include "PathPaintEngine.h"

#include <cmath>
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
		qreal actualdashsize;
		if(!dashPattern.empty())actualdashsize = dashPattern[0];
		for (int i = 0; i < polys.size(); ++i)
		{
			if(dashPattern.empty()) dev->addPath(transform.map(polys[i]));
			else
			{
				
				QPolygonF polytemp = transform.map(polys[i]);
				QPolygonF newpoly;
				newpoly.append(QPointF (polytemp[0].x(), polytemp[0].y()));
				for (int j = 1; j < polytemp.size(); ++j)
				{					
					newpoly.append(getPointAtLenght(polytemp[j-1], polytemp[j], actualdashsize));
				}
				dev->addPath(newpoly);
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
	dashPattern = state.pen().dashPattern();
	isCosmetic  = state.pen().isCosmetic(); 
}

qreal PathPaintEngine::getDistance(const QPointF &p1, const QPointF &p2)
{
	qreal testx1 = p1.x();
	qreal testy1 = p1.y();
	qreal testx2 = p2.x();
	qreal testy2 = p2.y();
	qreal a = 0.0;
	qreal b = 0.0;
	double c = 0.0;
	
	if(testx1 >= testx2) a = testx1 - testx2;
	else a = testx2 - testx1;
	if(testy1 >= testy2) b = testy1 - testy2;
	else b = testy2 - testy1;
	c = sqrt((double)(a*a+b*b));
	return (qreal) c;
}

QPointF PathPaintEngine::getPointAtLenght(const QPointF &p1, const QPointF &p2, qreal &l1)
{
	qreal testx1 = p1.x();
	qreal testy1 = p1.y();
	qreal testx2 = p2.x();
	qreal testy2 = p2.y();
	qreal lenghtp1p2 = getDistance(p1,p2);
	qreal lenghtdash = l1;
	l1 = lenghtp1p2 - lenghtdash;
	if (lenghtdash >= lenghtp1p2) 
	{
		l1 = lenghtdash - lenghtp1p2;
		return p1;
	}
	l1 = 0;
	qreal factor = (lenghtp1p2*lenghtp1p2)/(lenghtdash*lenghtdash);
	qreal a = testx1-testx2;
	qreal b = testy1-testy2;
	qreal aa = a*a;
	qreal bb = b*b;
	qreal as = aa*factor;
	qreal bs = bb*factor;
	return QPointF(sqrt(as),sqrt(bs));
	
}

