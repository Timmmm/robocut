#pragma once

#include <QGraphicsItem>

// A picture of the vinyl cutter so you can visualise where
// it will be cut.
class VinylCutterItem : public QGraphicsItem
{
public:
  QRectF boundingRect() const override;

  void paint(QPainter *painter,
             const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;
};

