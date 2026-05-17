#pragma once

#include <QGestureEvent>
#include <QGraphicsView>

class ZoomableGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit ZoomableGraphicsView(QWidget* parent = nullptr);
	ZoomableGraphicsView(QGraphicsScene* scene, QWidget* parent);

	~ZoomableGraphicsView() override;

protected:
	bool event(QEvent* event) override;

	bool gestureEvent(QGestureEvent* event);
	void panTriggered(QPanGesture* gesture);
	void pinchTriggered(QPinchGesture* gesture);

	void wheelEvent(QWheelEvent* event) override;
};
