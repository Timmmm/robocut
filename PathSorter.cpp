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

QList<QPolygonF> PathSorter::UnSort ()
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(pathToSort);
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

QList<QPolygonF> PathSorter::Sort ()
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(pathToSort);
	qSort(outpaths.begin(), outpaths.end(), MyLessThan);
	return outpaths;
}

QList<QPolygonF> PathSorter::TspSort ()
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(pathToSort);
	qSort(outpaths.begin(), outpaths.end(), MyLessThan);
	
	
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
	qreal bestdist = 0, tempdist = 0, zerodist=10000;
	bool outp = false, outp2 = false;
	QPolygonF zero = QPolygonF(QRectF(0.0,mediaHeight,0.0,0.0)); // able to change the start point
	int zerostuff=0;
	for (int i = 0; i < inpaths.size(); ++i)
	{
		tempdist = getDistance(zero,inpaths[i]);
		if (tempdist < zerodist)
		{
			zerodist = tempdist;
			zerostuff = i;
		}
	}
	// test the input
	bestdist = getTotalDistance(inpaths);
	cout<<"inpath: "<< bestdist << endl;
	QList<QPolygonF> outpaths, outpaths2, outpaths3;
	outpaths = QList<QPolygonF>(inpaths);
	outpaths.move(zerostuff,0);
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

	tempdist = getTotalDistance(outpaths);
	cout<<"outpath: "<< tempdist << endl;
	
	// finds better start and end
	outpaths2 = QList<QPolygonF>(outpaths);
	for (int id1 = 0; id1 < outpaths2.size(); ++id1)
	{	
		outpaths2.move(id1,0);
		for (int i = 0; i < (outpaths2.size()-1); ++i)
		{
			qreal dist=10000.0;
			int bestindex=i;
			for (int j = (i+1); j < outpaths2.size(); ++j)
			{
				if (getDistance(outpaths2[i],outpaths2[j]) < dist) 
				{
					dist = getDistance(outpaths2[i],outpaths2[j]);
					bestindex = j;
				}
			}
			if (dist != 0) outpaths2.swap((i+1),bestindex);
		}
		tempdist = 0;
		for (int i = 0; i < (outpaths2.size()-1); ++i)
		{
			tempdist = tempdist + getDistance(outpaths2[i],outpaths2[(i+1)]);
		}
		if (tempdist < bestdist) 
		{
			bestdist = tempdist;
			outp2 = true;
			outpaths3 = QList<QPolygonF>(outpaths2);
		}
		cout<<"outpath2_"<< id1 <<"_"<<":"<< tempdist << endl << "\033[1A";
	}
cout<< endl<<"best: "<< bestdist << endl;
	if (outp2) return outpaths3;
	if (outp) return outpaths;
	return inpaths;
}
