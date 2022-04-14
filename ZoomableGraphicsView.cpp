#include "ZoomableGraphicsView.h"

#include <QEvent>
#include <QGesture>
#include <QDebug>

ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent)
  : QGraphicsView(parent)
{
	grabGesture(Qt::PinchGesture);
}

ZoomableGraphicsView::ZoomableGraphicsView(QGraphicsScene* scene, QWidget* parent)
  : QGraphicsView(scene, parent)
{
	grabGesture(Qt::PinchGesture);
}

ZoomableGraphicsView::~ZoomableGraphicsView()
{
  
}

bool ZoomableGraphicsView::event(QEvent* event)
{
	switch (event->type())
	{
	case QEvent::Gesture:
		return gestureEvent(static_cast<QGestureEvent*>(event));
	default:
		break;
	}
	return QGraphicsView::event(event);
}

bool ZoomableGraphicsView::gestureEvent(QGestureEvent* event)
{
	if (QGesture* pan = event->gesture(Qt::PanGesture))
		panTriggered(static_cast<QPanGesture*>(pan));
	else if (QGesture* pinch = event->gesture(Qt::PinchGesture))
		pinchTriggered(static_cast<QPinchGesture*>(pinch));
	
	event->accept();
	
	return true;
}

void ZoomableGraphicsView::panTriggered(QPanGesture* gesture)
{
	(void)gesture;
}

void ZoomableGraphicsView::pinchTriggered(QPinchGesture* gesture)
{
	// Anchor under mouse for pinch events.
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	scale(gesture->scaleFactor(), gesture->scaleFactor());
	// Anchor in view centre for keyboard shortcuts.
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void ZoomableGraphicsView::wheelEvent(QWheelEvent* event)
{
	// For synthesized mouse events, i.e. ones not from a real mouse wheel
	// use the default implementation, which does nothing (we use pinch to zoom instead).
	if (event->source() != Qt::MouseEventNotSynthesized)
		return QGraphicsView::wheelEvent(event);
	
	// Anchor under mouse for mouse events.
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	if (event->pixelDelta().y() <= 0)
		scale(1.0/1.2, 1.0/1.2);
	else
		scale(1.2, 1.2);
	
	// Anchor in view centre for keyboard shortcuts.
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);
	
	event->accept();
}

