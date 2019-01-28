#pragma once

#include <QPoint>
#include <QPolygonF>

// Given a bezier curve defined by the points p1-p4, convert it
// to a polyline with the given fineness, and apprent it to poly.
void addBezierToPolygon(QPolygonF& poly,
                        const QPointF& p1, const QPointF& p2,
                        const QPointF& p3, const QPointF& p4,
                        qreal fineness);


