#include "PathScene.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

PathScene::PathScene(QObject* parent) : QGraphicsScene(parent)
{
}

void PathScene::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	emit mouseMoved(mouseEvent->scenePos());
}

void PathScene::mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
	emit mousePressed(mouseEvent->scenePos());
}
