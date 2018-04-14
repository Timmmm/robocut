#include "SvgRenderer.h"

#include <QPainter>
#include <QDebug>

#include "PathPaintDevice.h"

QList<QPolygonF> svgToPaths(QSvgRenderer& renderer, bool& clipped)
{
	auto viewBox = renderer.viewBoxF();

	PathPaintDevice pg(viewBox.width(), viewBox.height());
	QPainter p(&pg);

	renderer.render(&p, viewBox);
	
	clipped = pg.clipped();
	
	return pg.paths();
}

QPixmap svgToPreviewImage(QSvgRenderer& renderer, QSize dimensions)
{
	QPixmap pix(dimensions);
	pix.fill();
	
	auto viewBox = renderer.viewBoxF();
	
	auto scale = std::min(dimensions.width() / viewBox.width(),
	                      dimensions.height() / viewBox.height());
	
	QTransform transform;
	transform.scale(scale, scale);
	
	// TODO: There should be translation here.
//	transform.translate(???);
	
	bool clipped;
	auto paths = svgToPaths(renderer, clipped);	
	
	
	QPainter painter(&pix);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(QPen(Qt::black));
	painter.setTransform(transform);
	
	// Work out the scaling and offset for the paths.
	for (const auto& path : paths)
		painter.drawPolygon(path);

	return pix;	
}
