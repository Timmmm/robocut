#pragma once

#include <QList>
#include <QPolygonF>
#include <QSvgRenderer>

// Convert an SVG to a list of paths. Clipped is set to true if the SVG paths are outside its view box.
QList<QPolygonF> svgToPaths(QSvgRenderer& renderer,
                            bool& clipped);

// Make a preview image of an SVG. This renders the paths using a fixed-size pen so you'll be able to see
// them even if the preview size is small.
QPixmap svgToPreviewImage(QSvgRenderer& renderer, QSize dimensions);
