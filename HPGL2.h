#pragma once

#include <string>
#include "SvgRenderer.h"

// Render to HPGL2 (a text based format).
std::string renderToHPGL2(const QList<QPolygonF>& paths, double widthMm, double heightMm);

