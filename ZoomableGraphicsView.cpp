#include "ZoomableGraphicsView.h"

#include <QEvent>
#include <QGesture>
#include <QScroller>
#include <QDebug>

ZoomableGraphicsView::ZoomableGraphicsView(QWidget* parent)
  : QGraphicsView(parent)
{
	QScroller::grabGesture(this, QScroller::TouchGesture);
}

ZoomableGraphicsView::ZoomableGraphicsView(QGraphicsScene* scene, QWidget* parent)
  : QGraphicsView(scene, parent)
{
	QScroller::grabGesture(this, QScroller::TouchGesture);  
}

ZoomableGraphicsView::~ZoomableGraphicsView()
{
  
}

bool ZoomableGraphicsView::event(QEvent* event)
{
	switch (event->type())
	{
	case QEvent::ScrollPrepare:
	{
		qDebug() << "Scroll prepare";
		QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent*>(event);
		se->setViewportSize(size());
		QRectF br = sceneRect();
		se->setContentPosRange(QRectF(0, 0,
		                              qMax(qreal(0), br.width() - width()),
		                              qMax(qreal(0), br.height() - height())));
//		se->setContentPos(/* the current position */);
		se->accept();
		return true;
	}
	case QEvent::Scroll:
	{
		qDebug() << "Scroll";
		QScrollEvent *se = static_cast<QScrollEvent*>(event);
		centerOn(-se->contentPos() - se->overshootDistance());
		return true;
	}
//	case QEvent::Wheel:
//	{
//		// Anchor under the mouse pointer when using the mouse wheel.
//		// This doesn't quite work as nicely as I'd like because it clamps the scrolling
//		// precisely to the scene boundary. It's a bit hard to zoom to corners. Oh well.
//		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
//		QWheelEvent *we = static_cast<QWheelEvent*>(event);
//		if (we->delta() <= 0)
//			scale(1.0/1.2, 1.0/1.2);
//		else
//			scale(1.2, 1.2);

//		// Anchor in view centre for keyboard shortcuts.
//		setTransformationAnchor(QGraphicsView::AnchorViewCenter);
//		return true;
//	}
	default:
		break;
	}
	return QGraphicsView::event(event);
}
/*
bool ZoomableGraphicsView::gestureEvent(QGestureEvent* event)
{
	if (QGesture *pan = event->gesture(Qt::PanGesture))
		panTriggered(static_cast<QPanGesture*>(pan));
	else if (QGesture *pinch = event->gesture(Qt::PinchGesture))
		pinchTriggered(static_cast<QPinchGesture*>(pinch));
	
	return true;
}

void ZoomableGraphicsView::panTriggered(QPanGesture* gesture)
{
	gesture->
	if (gesture->state() == Qt::GestureFinished) {
		if (gesture->horizontalDirection() == QSwipeGesture::Left
		        || gesture->verticalDirection() == QSwipeGesture::Up) {
			qCDebug(lcExample) << "swipeTriggered(): swipe to previous";
			goPrevImage();
		} else {
			qCDebug(lcExample) << "swipeTriggered(): swipe to next";
			goNextImage();
		}
		update();
	}
}

void ZoomableGraphicsView::pinchTriggered(QPinchGesture* gesture)
{
	
}
*/
