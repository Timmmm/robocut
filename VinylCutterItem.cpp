#include "VinylCutterItem.h"

#include <QPainter>

QRectF VinylCutterItem::boundingRect() const
{
	return QRectF(-15.0, -25.0, 240.0, 40.0);
}

void VinylCutterItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	(void)option;
	(void)widget;

	QPen outline(QColor(50, 50, 50, 100));
	outline.setDashPattern({1.0, 1.0});
	outline.setWidthF(1.0);

	QBrush fill(QColor(200, 200, 200, 100));

	painter->setPen(outline);
	painter->setBrush(fill);
	painter->drawRoundedRect(-10, -25, 230, 35, 5, 5);

	painter->setPen(Qt::NoPen);
	painter->setBrush(QColor(50, 50, 50, 70));
	painter->drawRoundedRect(-5, -5, 220, 10, 5, 5);

	painter->setPen(QColor(20, 20, 20, 70));
	painter->setBrush(QColor(0, 0, 200, 70));
	painter->drawEllipse(-5, -5, 10, 10);

	painter->setPen(QColor(20, 20, 20, 100));
	painter->setBrush(Qt::NoBrush);
	painter->drawText(0, -10, "Vinyl Cutter");
}
