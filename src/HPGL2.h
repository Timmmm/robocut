#pragma once

#include <QPolygonF>
#include <string>

// Render to HPGL2 (a text based format).
std::string renderToHPGL2(const QList<QPolygonF>& paths, double widthMm, double heightMm);
