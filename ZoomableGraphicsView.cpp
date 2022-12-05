#include "ZoomableGraphicsView.h"

#include <QDebug>
#include <QEvent>
#include <QGesture>

ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
	grabGesture(Qt::PinchGesture);
}

ZoomableGraphicsView::ZoomableGraphicsView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent)
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
	// Note that this is never actually called for some reason.
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
	// For non-mouse events (e.g. pinch-zoom on a touchpad generates scroll events)
	// use the default implementation, which does nothing (we use pinch to zoom instead).
	qDebug() << "wheelEvent(): " << event->device()->type();

	double scaleBy = 1.0;

	if (event->hasPixelDelta())
	{
		scaleBy = exp(event->pixelDelta().y() * 0.002);
	}
	else
	{
		// Scroll wheel on Windows has no pixel delta for example.
		scaleBy = exp(event->angleDelta().y() * 0.002);
	}

	// Anchor under mouse for mouse events.
	setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	scale(scaleBy, scaleBy);

	// Anchor in view centre for keyboard shortcuts.
	setTransformationAnchor(QGraphicsView::AnchorViewCenter);

	event->accept();
}
