#ifndef PATHSORTER_H
#define PATHSORTER_H

#include <QGraphicsItem>

class PathSorter
{
public:
	PathSorter( );
	PathSorter(const QList<QPolygonF> inpaths, qreal mediaheight);
	virtual ~PathSorter( );

	void getQList (const QList<QPolygonF> inpaths);
	QList<QPolygonF> Sort ();
	QList<QPolygonF> UnSort ();
	QList<QPolygonF> TspSort ();
	void setMediaHeight(qreal mediaheight);
private:
	qreal mediaHeight;
	
protected:

private:
	QList<QPolygonF> MyFakeTSP(const QList<QPolygonF> inpaths);
	QList<QPolygonF> pathToSort;
	qreal getDistance(const QPolygonF &p1, const QPolygonF &p2);
	qreal getTotalDistance(const QList<QPolygonF> inpaths, int maxdepth = 0);
	static bool MyLessThan(const QPolygonF &p1, const QPolygonF &p2);
};

#endif // PATHSORTER_H
