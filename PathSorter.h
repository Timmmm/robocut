#pragma once

#include <QPolygonF>
#include <QList>

enum class PathSortMethod
{
	// Don't sort.
	None,
	
	// Try every method and choose the best one.
	Best,
	
	// Sort based on the first point in each polygon, by increasing y (and then increasing x in the
	// very unlikely event that y is equal).
	IncreasingY,
	
	// Sort based on bounding boxes - one path is cut before the other if their bounding boxes intersect
	// and its width or height is smaller. The idea being inside contours, like the two holes in an 8
	// are cut before the outside.
	InsideFirst,
	
	// Always cut the next nearest shape next.
	Greedy,
};

// Sort the paths to be cut using the given method. startingPoint is where the cutter starts, so it
// will try to cut a shape near there first (depending on the method).
//
// Closed polygons may be modified so that the start and end point is different (but they won't be split
// into multiple paths).
QList<QPolygonF> sortPaths(const QList<QPolygonF>& paths,
                           PathSortMethod method,
                           QPointF startingPoint);

