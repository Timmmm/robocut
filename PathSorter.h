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
	QList<QPolygonF> Sort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> Sort () {return Sort(pathToSort);};
	QList<QPolygonF> UnSort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> UnSort () {return UnSort(pathToSort);};
	QList<QPolygonF> TspSort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> TspSort () {return TspSort(pathToSort);};
	QList<QPolygonF> GroupTSP(const QList<QPolygonF> inpaths1, int groups = 3);
	QList<QPolygonF> GroupTSP(int groups = 3) {return GroupTSP(pathToSort, groups);};
	QList<QPolygonF> BbSort (const QList<QPolygonF> inpaths);
	QList<QPolygonF> BbSort () {return BbSort(pathToSort);};
	void setMediaHeight(qreal mediaheight);
private:
	QList<QPolygonF> pathToSort;
	qreal mediaHeight;
	
protected:

private:
	QList<QPolygonF> MyFakeTSP(const QList<QPolygonF> inpaths);
	qreal getDistance(const QPolygonF &p1, const QPolygonF &p2);
	qreal getTotalDistance(const QList<QPolygonF> inpaths, int maxdepth = 0);
	static bool MyLessThan(const QPolygonF &p1, const QPolygonF &p2);
};

#endif // PATHSORTER_H
