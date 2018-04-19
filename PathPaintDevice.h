#pragma once

#include <QPaintDevice>
#include <QList>
#include <QPolygonF>
#include <QSet>

class PathPaintEngine;

// Needed for QSet<QPolygonF>. Simply concatenates all the coordinates in a QByteArray and hashes that.
uint qHash(const QPolygonF& key);

// A paint device that just draws a list of paths in different colours.
class PathPaintDevice : public QPaintDevice
{
public:
	PathPaintDevice(double widthInMm, double heightInMm, double pixelsPerMm = 90.0/25.4);
	~PathPaintDevice() override;

	// Adds a path to the device.
	// Also automatically ignores duplicate paths, which QSvgPainter creates (I guess for the fill and stroke).
	// Paths are clipped (horribly) to the page. And if any are clipped, clipped() returns true.
	void addPath(const QPolygonF& path);

	// Get a list of paths.
	QList<QPolygonF> paths();

	QPaintEngine* paintEngine() const override;

protected:
	int metric(PaintDeviceMetric metric) const override;
private:
	mutable PathPaintEngine* engine;

	// The paths added.
	QList<QPolygonF> pagePaths;
	// Set of paths, so we can detect duplicates.
	QSet<QPolygonF> pagePathSet;

	double width;
	double height;

	double ppm;
};
