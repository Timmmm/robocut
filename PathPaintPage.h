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

#pragma once

// TODO: Rename this file to PathPaintDevice.h

#include <QPaintDevice>
#include <QList>
#include <QPolygonF>
#include <QSet>

class PathPaintEngine;

// Needed for QSet<QPolygonF>. Simply concatenates all the coordinates in a QByteArray and hashes that.
uint qHash(const QPolygonF& key);

class PathPaintDevice : public QPaintDevice
{
public:
	PathPaintDevice(double widthInMm, double heightInMm, double pixelsPerMm = 90.0/25.4);
	~PathPaintDevice();

	// Adds a path to the device.
	// Also automatically ignores duplicate paths, which QSvgPainter creates (I guess for the fill and stroke).
	// Paths are clipped (horribly) to the page. And if any are clipped, clipped() returns true.
	void addPath(const QPolygonF& path);

	// Get a list of paths.
	QList<QPolygonF> paths();

	// Returns true if any paths had to be clipped.
	bool clipped() const;

	QPaintEngine* paintEngine() const;


protected:
	int metric(PaintDeviceMetric metric) const;
private:
	mutable PathPaintEngine* engine;

	// The paths added.
	QList<QPolygonF> pagePaths;
	// Set of paths, so we can detect duplicates.
	QSet<QPolygonF> pagePathSet;

	bool pathsClipped;

	double width;
	double height;

	double ppm;
};
