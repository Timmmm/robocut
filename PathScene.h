#pragma once

#include <QGraphicsScene>
#include <QPointF>

// This is a normal QGraphicsScene but it sends mouse events publicly.
class PathScene : public QGraphicsScene
{
	Q_OBJECT

public:
	explicit PathScene(QObject* parent = nullptr);

signals:
	void mouseMoved(QPointF p);
	void mousePressed(QPointF p);

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
};
