#include "Bezier.h"

void addBezierToPolygon(QPolygonF& poly,
                        const QPointF& p1, const QPointF& p2,
                        const QPointF& p3, const QPointF& p4,
                        qreal angleThreshold,
                        qreal distanceThreshold)
{
	// TODO: Calculate N somehow.
	const int N = 10;
	for (int i = 1; i < N; ++i)
	{
		qreal t = i / (N-1.0);
		qreal s = 1.0 - t;
		
		QPointF x = s*s*s * p1 + 3.0 * s * t * (s * p2 + t * p3) + t*t*t * p4;
		
		poly.append(x);
	}
}
