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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
	QVector<qreal> dashPattern;
	bool isCosmetic;
	qreal getDistance(const QPointF &p1, const QPointF &p2);
	QPointF getPointAtLenght(const QPointF &p1, const QPointF &p2, qreal &l1);
};

