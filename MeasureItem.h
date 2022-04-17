#pragma once

#include <QDebug>
#include <QGraphicsItem>
#include <QPainter>

#include <cmath>

class MeasureItem : public QGraphicsItem
{
public:
	void setVec(QPointF v);

	enum class State
	{
		Free,
		OneEndAttached,
		BothEndsAttached,
	};

	void setState(State s);

	State state() const;

	QRectF boundingRect() const override;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
	QPointF vec = QPointF(30.0, 0.0);
	State endStates = State::Free;
};
