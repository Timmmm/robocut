/***************************************************************************
 *   This file is part of Robocut.                                         *
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

#pragma once

#include <QGraphicsItem>

class PathSorter
{
public:
	PathSorter( );
	PathSorter(const QList<QPolygonF> inpaths, qreal mediaheight);
	virtual ~PathSorter( );

	void getQList (const QList<QPolygonF> inpaths);
	QList<QPolygonF> Sort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> Sort () {return Sort(pathToSort);}
	QList<QPolygonF> UnSort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> UnSort () {return UnSort(pathToSort);}
	QList<QPolygonF> BestSort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> BestSort () {return BestSort(pathToSort);}
	QList<QPolygonF> GroupTSP(const QList<QPolygonF> inpaths1, int groups = 3);
	QList<QPolygonF> GroupTSP(int groups = 3) {return GroupTSP(pathToSort, groups);}
	QList<QPolygonF> BbSort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> BbSort () {return BbSort(pathToSort);}
	void setMediaHeight(qreal mediaheight);
private:
	QList<QPolygonF> pathToSort;
	qreal mediaHeight;
	
protected:

private:
	QList<QPolygonF> MyFakeTSP(const QList<QPolygonF> inpaths);
	qreal getDistance(const QPolygonF &p1, const QPolygonF &p2);
	qreal getTotalDistance(const QList<QPolygonF> inpaths, int maxdepth = 0);
	static bool MyLessThan(const QPolygonF &p1, const QPolygonF &p2);
};

