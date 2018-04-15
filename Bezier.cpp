#include "Bezier.h"

#include <cmath>

#include <QDebug>

namespace {

// If x is a point on the Bezier parameterised by t, then
//
//   a = d^2 x / dt^2
//
// And
//
//   curvinessness = sqrt( integral(dot(a, a) dt, from 0 to 1) )
//
// Something like that anyway. This seems to work.
//
qreal curvinessness(const QPointF& p0, const QPointF& p1,
                    const QPointF& p2, const QPointF& p3)
{
	auto c1 = -p0 + 3.0 * p1 - 3.0 * p2 + p3;
	auto c2 = p0 - 2.0 * p1 + p2;
	auto norm = 2.0 * QPointF::dotProduct(c1, c1)
	            + QPointF::dotProduct(c1, c2)
	            + QPointF::dotProduct(c2, c2);
	return std::sqrt(norm);
}

}

void addBezierToPolygon(QPolygonF& poly,
                        const QPointF& p0, const QPointF& p1,
                        const QPointF& p2, const QPointF& p3,
                        qreal fineness)
{
	int N = (curvinessness(p0, p1, p2, p3) * fineness);
	
	// Add reasonable bounds.
	if (N < 4)
		N = 4;
	if (N > 32)
		N = 32;
	
	// Evaluate the Bezier directly. Equal spacing on t seems to give closer
	// spacing on curvier parts which is what we want. I like this more than
	// the de Casteljau algorithm.
	for (int i = 1; i < N; ++i)
	{
		qreal t = i / (N-1.0);
		qreal s = 1.0 - t;
		
		QPointF x = s*s*s * p0 + 3.0 * s * t * (s * p1 + t * p2) + t*t*t * p3;
		
		poly.append(x);
	}
}
