#pragma once

#include <QPoint>
#include <QPolygonF>

void addBezierToPolygon(QPolygonF& poly,
                        const QPointF& p1, const QPointF& p2,
                        const QPointF& p3, const QPointF& p4,
                        qreal fineness);


