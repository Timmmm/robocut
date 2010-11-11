#include "PathSorter.h"
#include <cmath>
#include <QSvgRenderer>
#include <iostream>

using namespace std;

PathSorter::PathSorter ( ) {}
PathSorter::PathSorter (const QList<QPolygonF> inpaths, qreal mediaheight ) 
{
	getQList(inpaths);
	mediaHeight = mediaheight;
}
PathSorter::~PathSorter ( ) { }

void PathSorter::getQList (const QList<QPolygonF> inpaths)
{
	pathToSort = QList<QPolygonF>(inpaths);
}

void PathSorter::setMediaHeight(qreal mediaheight)
{
	mediaHeight = mediaheight;
}

QList<QPolygonF> PathSorter::UnSort (const QList<QPolygonF> inpaths)
{
	cout<<" UnSort inpath: "<< getTotalDistance(inpaths) << endl;
	QList<QPolygonF> outpaths = QList<QPolygonF>(inpaths);
	return outpaths;
}

bool PathSorter::MyLessThan(const QPolygonF &p1, const QPolygonF &p2)
{
	qreal testx1 = p1[0].x();
	qreal testx2 = p2[0].x();
	qreal testy1 = p1[0].y();
	qreal testy2 = p2[0].y();
	if(testy1 == testy2) return testx1 > testx2;
	else return testy1 > testy2;
}

QList<QPolygonF> PathSorter::Sort (const QList<QPolygonF> inpaths)
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(inpaths);
	qSort(outpaths.begin(), outpaths.end(), MyLessThan);
	cout<<"Sort outpaths: "<< getTotalDistance(outpaths) << endl;
	return outpaths;
}

QList<QPolygonF> PathSorter::TspSort (const QList<QPolygonF> inpaths)
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(inpaths);
	cout<<"TSPSort outpaths: "<< getTotalDistance(outpaths) << endl;
	return MyFakeTSP(outpaths);
}

QList<QPolygonF> PathSorter::BbSort (const QList<QPolygonF> inpaths)
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(inpaths);
	qSort(outpaths.begin(), outpaths.end(), MyLessThan);
	
	
	return inpaths;
}

QList<QPolygonF> PathSorter::GroupTSP(const QList<QPolygonF> inpaths1)
{
	QList<QPolygonF> inpaths = Sort(inpaths1);
	QList<QList< QPolygonF> > listlistpath;
	QList<QPolygonF> temppaths;
	
	int inps = inpaths.size(), group = 0;
	int inpsparts = inps / 3;
	
	for (int i = 0; i < inps; i++)
	{
		if(i>=inpsparts)
		{
			listlistpath.append(temppaths);
			inpsparts += inps / 3;
			group++;
			temppaths = QList<QPolygonF>() ;
		}
		temppaths.append(inpaths[i]);
	}
	listlistpath.append(temppaths);

	for (int i = 0; i < listlistpath.size(); i++)
	{
		listlistpath[i] = TspSort(listlistpath[i]);
	}
	
	QList<QPolygonF> outpaths;
	for (int i = 0; i < listlistpath.size(); i++)
	{
		for (int j = 0; j < listlistpath[i].size(); j++)
		{
			outpaths.append(listlistpath[i][j]);
		}
	}
	
	cout<<"GroupTSP outpaths: "<< getTotalDistance(outpaths) << endl;
	return outpaths;
}

qreal PathSorter::getDistance(const QPolygonF &p1, const QPolygonF &p2)
{
	qreal testx1 = p1.last().x();
	qreal testy1 = p1.last().y();
	qreal testx2 = p2.first().x();
	qreal testy2 = p2.first().y();
	qreal a = 0.0;
	qreal b = 0.0;
	double c = 0.0;
	
	if(testx1 >= testx2) a = testx1 - testx2;
	else a = testx2 - testx1;
	if(testy1 >= testy2) b = testy1 - testy2;
	else b = testy2 - testy1;
	c = sqrt((double)(a*a+b*b));
	return (qreal) c;
}

qreal PathSorter::getTotalDistance(const QList<QPolygonF> inpaths, int maxdepth)
{
	if (maxdepth >= inpaths.size()-1) maxdepth = inpaths.size()-1;
	if (maxdepth <= 0) maxdepth = inpaths.size()-1;
	QPolygonF zero = QPolygonF(QRectF(0.0,mediaHeight,0.0,0.0)); // able to change the start point
	qreal dist = getDistance(zero,inpaths[0]);
	for (int i = 0; i < maxdepth; ++i)
	{
		dist = dist + getDistance(inpaths[i],inpaths[(i+1)]);
	}
	return dist;
}

QList<QPolygonF> PathSorter::MyFakeTSP(const QList<QPolygonF> inpaths)
{
	QPolygonF zero = QPolygonF(QRectF(0.0,mediaHeight,0.0,0.0)); // able to change the start point

	QList<QPolygonF> outpaths = QList<QPolygonF>(inpaths);

	// find the shortest path
	for (int i = 0; i < (outpaths.size()-1); ++i)
	{
		qreal dist=10000.0;
		int bestindex=i;
		for (int j = (i+1); j < outpaths.size(); ++j)
		{
			if (getDistance(outpaths[i],outpaths[j]) < dist) 
			{
				dist = getDistance(outpaths[i],outpaths[j]);
				bestindex = j;
			}
		}
		if (dist != 0) outpaths.swap((i+1),bestindex);
	}
	return outpaths;
}
