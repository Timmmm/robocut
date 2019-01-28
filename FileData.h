#pragma once

#include <QString>
#include <QList>
#include <QPolygonF>

// This struct stores the data that is loaded from a file.
struct FileData
{
	// The polygons to cut, in mm.
	QList<QPolygonF> paths;
	// The page size in mm.
	QSizeF mediaSize;
};
