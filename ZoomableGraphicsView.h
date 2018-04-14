#pragma once

#include <QGraphicsView>
#include <QGestureEvent>

class ZoomableGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit ZoomableGraphicsView(QWidget* parent = nullptr);
	ZoomableGraphicsView(QGraphicsScene* scene, QWidget* parent);
	
	~ZoomableGraphicsView();
	
protected:
	bool event(QEvent* event) override;
	/*
	bool gestureEvent(QGestureEvent* event);
	void panTriggered(QPanGesture* gesture);
	void pinchTriggered(QPinchGesture* gesture);
	
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;*/
	
};
