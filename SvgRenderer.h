#pragma once

#include <QList>
#include <QPolygonF>
#include <QSvgRenderer>

#include "Result.h"

struct SvgRender
{
	// The shapes, in user units.
	//
	// User units can be anything - in Inkscape by default 1 unit is 1 mm
	// but this can be changed in the document settings and isn't guaranteed.
	QList<QPolygonF> showpaths;
	QList<QPolygonF> cutpaths;

	// The view box (the visible portal of the SVG) in user units.
	QRectF viewBox;

	// The SVG width and height attributes if present.
	QString widthAttribute;
	QString heightAttribute;

	//The Cut Layer only
	QString cutElementId = QString();

	//The Registration marks layer only
	QString markElementId = QString();
	QRectF markPosition = QRectF();
	QPointF markOffset;
	double markStroke;

	// The width and height in mm if they have been specified. This is calculated
	// from the width= and height= attributes, if they have physical units. If they
	// are in user units then it is assumed to be mm. If they are in % or are not present
	// then the view box size is used and is assumed to be in mm.
	// If they are in pixels then they are converted at 96 DPI per the SVG spec.
	double widthMm;
	double heightMm;

	// Whether or not the SVG contains tspans with x or y attributes.
	// These are used by Inkscape for multiline text but they aren't part
	// of SVG Tiny so Qt doesn't support them.
	bool hasTspanPosition = false;
};

// Convert an SVG to a list of paths.
SResult<SvgRender> svgToPaths(const QString& filename, bool searchForTspans);

// Make a preview image of an SVG. This renders the paths using a fixed-size pen so you'll be able to see
// them even if the preview size is small.
QPixmap svgToPreviewImage(const QString& filename, QSize dimensions);
