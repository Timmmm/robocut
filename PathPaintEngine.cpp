#include "PathPaintEngine.h"

#include <cmath>
#include <QDebug>
#include "Bezier.h"

namespace {

	QList<QPolygonF> polygonToDashes(const QPolygonF& poly, const QVector<qreal>& dashPattern)
	{
		// Convert a polygon to a list of dash polygons using dashPattern.
		
		// If there's no valid dash pattern, just return the original polygon as one long "dash".	
		if (dashPattern.size() < 2)
			return {poly};
		
		QList<QPolygonF> dashes;
		if (poly.empty())
			return dashes;
		
		// Index into the dashPattern. dashPattern is an even-sized list of dash lengths
		// where the first entry is a dash, and the second is a space and so on.
		int dashIdx = 0;
		
		// Length remaining in this dash.
		qreal dashLengthLeft = dashPattern[0];
		
		QPointF pos = poly[0];
	
		// Polygon for this dash.
		QPolygonF dashPoly;
		dashPoly.append(pos);
		
		for (int polyIdx = 1; polyIdx < poly.size();)
		{
			QPointF nextPos = poly[polyIdx];
			
			QLineF line(pos, nextPos);
			auto len = line.length();
			
			// If this line is shorter than the remaining dash length, just add it.
			if (len < dashLengthLeft)
			{
				if (dashIdx % 2 == 0)
					dashPoly.append(nextPos);
	
				dashLengthLeft -= len;
				++polyIdx;
			}
			else
			{
				// Otherwise clip it at the line length and start a new dash.
				line.setLength(dashLengthLeft);
				if (dashIdx % 2 == 0)
				{
					dashPoly.append(line.p2());
					// TODO: Remove zero-length polys. They act weirdly.
					dashes.append(dashPoly);
					dashPoly.clear();
				}
				
				dashIdx = (dashIdx + 1) % dashPattern.size();
				dashLengthLeft = dashPattern[dashIdx];
			}
			
			pos = nextPos;
		}
		
		if (!dashPoly.empty())
			dashes.append(dashPoly);
		
		return dashes;	
	}
	
	
	// This is a copy of QPainterPath::toSubpathPolygons with the tiny modification
	// of exposing the bezier flattening threshold.
	QList<QPolygonF> toSubpathPolygons(const QPainterPath &path,
	                                   const QTransform &matrix,
	                                   qreal fineness)
	{
		QList<QPolygonF> flatCurves;
		if (path.isEmpty())
			return flatCurves;
		QPolygonF current;
		for (int i = 0; i < path.elementCount(); ++i) {
			const QPainterPath::Element &e = path.elementAt(i);
			switch (e.type) {
			case QPainterPath::MoveToElement:
				if (current.size() > 1)
					flatCurves += current;
				current.clear();
				current.reserve(16);
				current += QPointF(e.x, e.y) * matrix;
				break;
			case QPainterPath::LineToElement:
				current += QPointF(e.x, e.y) * matrix;
				break;
			case QPainterPath::CurveToElement: {
				Q_ASSERT(path.elementAt(i+1).type == QPainterPath::CurveToDataElement);
				Q_ASSERT(path.elementAt(i+2).type == QPainterPath::CurveToDataElement);
				addBezierToPolygon(current,
				                   QPointF(path.elementAt(i-1).x, path.elementAt(i-1).y) * matrix,
				                   QPointF(e.x, e.y) * matrix,
				                   QPointF(path.elementAt(i+1).x, path.elementAt(i+1).y) * matrix,
				                   QPointF(path.elementAt(i+2).x, path.elementAt(i+2).y) * matrix,
				                   fineness);
				i+=2;
				break;
			}
			case QPainterPath::CurveToDataElement:
				Q_ASSERT(false && "QPainterPath::toSubpathPolygons(), bad element type");
				break;
			}
		}
		if (current.size()>1)
			flatCurves += current;
		return flatCurves;
	}

}

// We need to say that at least transforms are supported, otherwise transformed
// elements are ignored. I just say everything is supported and we can ignore
// most things.
PathPaintEngine::PathPaintEngine() : QPaintEngine(QPaintEngine::AllFeatures)
{	
}

bool PathPaintEngine::begin(QPaintDevice* pdev)
{
	dev = dynamic_cast<PathPaintDevice*>(pdev);
	if (!dev)
		qWarning("PathPaintEngine: unsupported target device.");

	setActive(true);
	return true;
}

void PathPaintEngine::drawPath(const QPainterPath& path)
{
	if (dev == nullptr)
		return;

	if (zeroWidthPen)
		return;

	// TODO: Make the 30 configurable.
	QList<QPolygonF> polys = toSubpathPolygons(path, transform, 30.0);
	
	for (const auto& poly : polys)
	{
		// Convert polygon to dashes. If dashPattern is empty, only the original polygon
		// is returned.
		auto dashes = polygonToDashes(poly, dashPattern);
		for (const auto &dash : dashes)
			dev->addPath(dash);
	}
}

void PathPaintEngine::drawPixmap(const QRectF& r, const QPixmap& pm, const QRectF& sr)
{
	// Nop.
	(void)r;
	(void)pm;
	(void)sr;
}

void PathPaintEngine::drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode)
{
	// Mode is only needed for fills.
	(void)mode;

	if (dev == nullptr)
		return;

	QPolygonF p(pointCount + 1);
	for (int i = 0; i < pointCount; ++i)
		p[i] = points[i];
	
	// Close the polygon.
	p[pointCount] = p[0];
	
	dev->addPath(transform.map(p));
}

bool PathPaintEngine::end()
{
	setActive(false);
	if (dev == nullptr)
		return false;
	dev = nullptr;
	return true;
}

QPaintEngine::Type PathPaintEngine::type() const
{
	// Return the type of the paint engine (X11, PDF, etc). This is the first custom type ID.
	return QPaintEngine::User;
}

void PathPaintEngine::updateState(const QPaintEngineState& state)
{
	if (state.state() & DirtyTransform)
		transform = state.transform();
	if (state.state() & DirtyPen)
	{
		dashPattern = state.pen().dashPattern();
		zeroWidthPen = state.pen().widthF() == 0.0;
	}
}

