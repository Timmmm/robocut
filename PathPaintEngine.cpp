/***************************************************************************
 *   This file is part of Robocut.                                         *
 *   Copyright (C) 2010 Tim Hutt <tdhutt@gmail.com>                        *
 *   Copyright (C) 2010 Markus Schulz <schulz@alpharesearch.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

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

bool approximatelyEqual(qreal a, qreal b, qreal epsilon)
{
    return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool essentiallyEqual(qreal a, qreal b, qreal epsilon)
{
    return fabs(a - b) <= ( (fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyGreaterThan(qreal a, qreal b, qreal epsilon)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyLessThan(qreal a, qreal b, qreal epsilon)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
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
			if(dashPattern.empty()) dev->addPath(transform.map(polys[i]));
			else
			{
				QPolygonF polytemp = transform.map(polys[i]), newpoly;
				int  dashtoggle = 1, dashi=0, j = 0;
				qreal actualdashsize = dashPattern[dashi];
				QPointF origin = QPointF(polytemp[j]), testp;
				j++; 
				do
				{
					newpoly = QPolygonF();
					newpoly.append(origin);
					do
					{
						testp = polytemp[j];
						origin = QPointF(getPointAtLenght(QPointF(origin), polytemp[j], actualdashsize));
						if (essentiallyEqual(origin.x(), polytemp[j].x(), 0.01 ) && approximatelyEqual(origin.y(), polytemp[j].y(),0.01) && j+1 < polytemp.size()) 
						{
							origin = polytemp[j];
							int tempz = polytemp.size();
							j++;
							testp =  polytemp[j];
						}
						newpoly.append(origin);
					
					}while(definitelyGreaterThan(actualdashsize,0.0,0.1) && testp!=origin);
					if(dashtoggle == 1)
					{
						dev->addPath(newpoly);
					}
					dashtoggle = dashtoggle * -1;
					dashi++;
					if(dashi >= dashPattern.size()) dashi=0;
					actualdashsize = dashPattern[dashi];
				}while(!essentiallyEqual(origin.x(), polytemp[j].x(), 0.001 ) || !essentiallyEqual(origin.y(), polytemp[j].y(),0.001));
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
	if (definitelyLessThan(lenghtp1p2, lenghtdash, 0.01)) 
	{
		l1 = lenghtdash - lenghtp1p2;
		return p2;
	}
	if (definitelyGreaterThan(lenghtdash, lenghtp1p2, 0.01)) 
	{
		l1 = lenghtdash - lenghtp1p2;
		return p1;
	}
	l1 = 0;
	qreal factor = lenghtp1p2/lenghtdash;
	qreal a = testx1-testx2;
	qreal b = testy1-testy2;
	qreal aa = a/factor;
	qreal bb = b/factor;
	QPointF ret = QPointF(testx1-aa,testy1-bb);
	return ret;
	
}

