#include "PathSorter.h"

#include <cmath>
#include <iostream>

#include <nanoflann.hpp>

#include <QSvgRenderer>

// Return the distance travelled by the cutter when it isn't
// cutting paths.
double pathSortDistance(const QList<QPolygonF>& paths,
                        QPointF startingPoint)
{
	double distance = 0.0;
	for (const auto& path : paths)
	{
		distance += QLineF(startingPoint, path.front()).length();
		startingPoint = path.back();
	}
	// Shorter distance is higher quality.
	return distance;
}

QPolygonF rotatePolygonStart(const QPolygonF& poly, int newStart)
{
	if (poly.isEmpty())
		return {};

	if (newStart == 0)
		return poly;
	
	// If the polygon is closed then all we can do is reverse the direction.
	// This is based on which end is closest to the new start point.
	if (!poly.isClosed())
	{
		if (QLineF(poly.front(), poly[newStart]).length() > QLineF(poly.back(), poly[newStart]).length())
		{
			QPolygonF rotated(poly.size());
			for (int i = 0; i < poly.size(); ++i)
				rotated[i] = poly[poly.size() - i - 1];
			return rotated;
		}

		return poly;
	}
	
	// It's a closed polygon. Wrap the points ignoring the last repeated point.
	QPolygonF rotated(poly.size());
	
	for (std::size_t i = 0; i < poly.size()-1; ++i)
		rotated[i] = poly[(i + newStart) % (poly.size() - 1)];
	
	// And then set the repeated point.
	rotated[poly.size() - 1] = rotated[0];
	
	return rotated;
}

// Return the index of the point in the polygon with the largest Y coordinate (+Y is down).
int highestIndex(const QPolygonF& poly)
{
	int maxIdx = 0;
	qreal maxY = std::numeric_limits<qreal>::lowest();
	for (std::size_t i = 0; i < poly.size(); ++i)
	{
		if (poly[i].y() > maxY)
		{
			maxIdx = i;
			maxY = poly[i].y();
		}
	}
	return maxIdx;
}

// Return the index of the point in the polygon closest to p.
int closestIndex(const QPolygonF& poly, QPointF p)
{
	int minIdx = 0;
	qreal minDist = std::numeric_limits<qreal>::max();
	for (std::size_t i = 0; i < poly.size(); ++i)
	{
		auto dist = QLineF(poly[i], p).length();
		if (dist < minDist)
		{
			minIdx = i;
			minDist = dist;
		}
	}
	return minIdx;
}

// Sort paths by increasing Y... but ... well, +Y is down so this actually sorts
// them by decreasing Y coordinate.
QList<QPolygonF> sortPathsIncreasingY(const QList<QPolygonF>& paths,
		                              QPointF startingPoint)
{
	(void)startingPoint;
	
	// Basically this tries to avoid going backwards as much as possible.
	
	struct IndexAndBottom
	{
		// Index of the path in `paths`
		int pathIndex;
		// Index of the point in the path with the down-most Y coordinate.
		int bottomIndex;
	};
	
	// Get the indices of the lowest points.
	QList<IndexAndBottom> indices;
	indices.reserve(paths.size());
	
	for (int i = 0; i < paths.size(); ++i)
		indices.append({i, highestIndex(paths[i])});
	
	qSort(indices.begin(), indices.end(),
	      [&](const IndexAndBottom& a, const IndexAndBottom& b) {
		return paths[a.pathIndex][a.bottomIndex].y() > paths[b.pathIndex][b.bottomIndex].y();
	});
	
	QList<QPolygonF> sorted;
	sorted.reserve(paths.size());
	
	// And rotate the polygons so that the lowest point is at the start.
	for (auto& idx : indices)
		sorted.append(rotatePolygonStart(paths[idx.pathIndex], idx.bottomIndex));
	
	return sorted;
}

QList<QPolygonF> sortPathsInsideFirst(const QList<QPolygonF>& paths,
                                      QPointF startingPoint)
{
	// Sort based on bounding boxes - one path is cut before the other if their bounding boxes intersect
	// and its width or height is smaller. The idea being inside contours, like the two holes in an 8
	// are cut before the outside.
	
	struct IndexAndBB
	{
		// Index of the path in `paths`
		int pathIndex;
		// The bounding box of the path.
		QRectF boundingBox;
	};
	
	// Get the indices of the lowest points.
	QList<IndexAndBB> indices;
	indices.reserve(paths.size());
	
	for (int i = 0; i < paths.size(); ++i)
		indices.append({i, paths[i].boundingRect()});
	
	qSort(indices.begin(), indices.end(),
	      [&](const IndexAndBB& a, const IndexAndBB& b) {
		if (a.boundingBox.intersects(b.boundingBox))
			return a.boundingBox.width() + a.boundingBox.height() < b.boundingBox.width() + b.boundingBox.height();
		
		return a.boundingBox.bottom() < b.boundingBox.bottom();
	});
	
	QList<QPolygonF> sorted;
	sorted.reserve(paths.size());
	
	for (auto& idx : indices)
	{
		auto closestPointIndex = closestIndex(paths[idx.pathIndex], startingPoint);
		auto poly = rotatePolygonStart(paths[idx.pathIndex], closestPointIndex);
		sorted.append(poly);
		startingPoint = poly.back();
	}
	
	return sorted;
}

// A data structure that stores a set of points, and the index
// of the polygon they are from and the index of the point within that polygon.
class VertexCloud
{
public:
	struct Point
	{
		Point(qreal x, qreal y, int polygon, int vertex)
		    : x(x), y(y), polygon(polygon), vertex(vertex) {}
		
		qreal x;
		qreal y;
		
		int polygon;
		int vertex;
	};

	std::vector<Point> points;

	inline size_t kdtree_get_point_count() const
	{
		return points.size();
	}
	
	inline qreal kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim == 0)
			return points[idx].x;
		if (dim == 1)
			return points[idx].y;
		return 0;
	}

	// Unused bounding box calculation.
	template <class BBOX>
	bool kdtree_get_bbox(BBOX&) const { return false; }
};


QList<QPolygonF> sortPathsGreedy(const QList<QPolygonF>& paths,
                                 QPointF startingPoint)
{
	// Always cut the next nearest shape next.
	
	// To do this vaguely efficiently we create a spatial index of points
	// where each point records the polygon that it is a part of. We also store
	// a map from polygon number to the list of points in the spatial index.
	//
	// We search for the nearest point, then add that polygon and remove all of
	// its points from the spatial index.
	
	using namespace nanoflann;
	
	QList<QPolygonF> sorted;
	sorted.reserve(paths.size());
	
	VertexCloud vertices;
	
	typedef KDTreeSingleIndexDynamicAdaptor<
		L2_Simple_Adaptor<qreal, VertexCloud>,
		VertexCloud,
		2> kd_tree_adapter;
	
	kd_tree_adapter index(2, vertices);
	
	QVector<int> pathOffset(paths.size());
	int offs = 0;
	
	// Add the points to the dataset.
	for (int p = 0; p < paths.size(); ++p)
	{
		pathOffset[p] = offs;
		offs += paths[p].size();
		
		for (int v = 0; v < paths[p].size(); ++v)
		{
			auto pt = paths[p][v];
			vertices.points.emplace_back(pt.x(), pt.y(), p, v);
		}
	}
	
	// Add all the points from the dataset to the index.
	// Note! This is [begin, end] not [begin, end) as you might expect!
	if (!vertices.points.empty())
		index.addPoints(0, vertices.points.size() - 1);
	
	while (sorted.size() < paths.size())
	{
		// Find the nearest point 
		const size_t num_results = 1;
		size_t ret_index = 0;
		qreal out_dist_sqr = 0;
		KNNResultSet<qreal> resultSet(num_results);
		resultSet.init(&ret_index, &out_dist_sqr);
		std::vector<qreal> queryPoint{startingPoint.x(), startingPoint.y()};
		index.findNeighbors(resultSet,
		                    queryPoint.data(),
		                    SearchParams());

		const auto& closest = vertices.points[ret_index];
		
		// Add the polygon and rotate it appropriately.
		sorted.append(rotatePolygonStart(paths[closest.polygon], closest.vertex));
		
		// Remove those points from the index.
		for (int i = pathOffset[closest.polygon];
		     i < pathOffset[closest.polygon] + paths[closest.polygon].size();
		     ++i)
			index.removePoint(i);
		
		startingPoint = sorted.back().back();
	}
	
	return sorted;
}

QList<QPolygonF> sortPaths(const QList<QPolygonF>& paths,
                           PathSortMethod method,
                           QPointF startingPoint)
{
	switch (method)
	{
	case PathSortMethod::Best:
	{
		QList<QList<QPolygonF>> pathsSorted{
		    sortPathsIncreasingY(paths, startingPoint),
		    sortPathsInsideFirst(paths, startingPoint),
		    sortPathsGreedy(paths, startingPoint),
		    paths,
		};
		
		// Determine path quality by the distance travelled by the cutter
		// while it isn't cutting (shorter distance is higher quality).
		QList<double> qualities;
		for (const auto& p : pathsSorted)
			qualities.append(-pathSortDistance(p, startingPoint));
		
		auto bestIdx = std::distance(qualities.begin(),
		                             std::max_element(qualities.begin(), qualities.end()));
		
		return pathsSorted[bestIdx];
	}
	
	case PathSortMethod::IncreasingY:
		return sortPathsIncreasingY(paths, startingPoint);
	
	case PathSortMethod::InsideFirst:
		return sortPathsInsideFirst(paths, startingPoint);
	
	case PathSortMethod::Greedy:
		return sortPathsGreedy(paths, startingPoint);
		
	default:
		break;
	}
	
	return paths;
}
