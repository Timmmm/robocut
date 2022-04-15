#include "MeasureItem.h"

void MeasureItem::setVec(QPointF v)
{
  prepareGeometryChange();
  vec = v;
}

void MeasureItem::setState(MeasureItem::State s)
{
  endStates = s;
}

MeasureItem::State MeasureItem::state() const
{
  return endStates;
}

QRectF MeasureItem::boundingRect() const
{
  QRectF r(std::min(0.0, vec.x()), std::min(0.0, vec.y()),
           std::fabs(vec.x()), std::fabs(vec.y()));
  r.adjust(-15.0, -15.0, 80.0, 15.0);
  qDebug() << "Bounding rect: " << r;
  return r;
}

void MeasureItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  (void)option;
  (void)widget;

  QPointF normal(-vec.y(), vec.x());
  auto len = std::hypot(normal.x(), normal.y());
  if (len == 0.0)
  {
    return;
  }

  normal /= len;

  QVector<QPointF> outline = {
    QPointF(),
    vec,
    vec + normal * 10.0,
    normal * 10.0,
  };

  QPen outlinePen(Qt::darkGray);
  outlinePen.setCosmetic(true);
  painter->setPen(outlinePen);
  painter->setBrush(QBrush(Qt::yellow));
  painter->drawPolygon(outline.data(), static_cast<int>(outline.size()));

  QPen markPen(Qt::darkGray);
  markPen.setCapStyle(Qt::PenCapStyle::FlatCap);
  painter->setPen(markPen);

  // Draw some ruler marks.
  for (double d = 10.0; d <= len - 5.0; d += 10.0)
  {
    auto p1 = vec * d / len;
    auto p2 = p1 + normal * 5.0;
    painter->drawLine(p1, p2);
  }

  // Show the length.
  if (endStates != State::Free)
  {
    painter->setFont(QFont("Helvetica", 10.0));
    painter->drawText(vec + QPointF(5.0, 5.0), QString::number(len, 'f', 1) + " mm");
  }
}
