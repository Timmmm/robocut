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
	return outpaths;
}

QList<QPolygonF> PathSorter::BestSort (const QList<QPolygonF> inpaths)
{
	qreal inpath = getTotalDistance(inpaths);
	qreal sortpath = inpath;
	qreal tsppath = inpath, temppath;
	qreal tsppath2 = inpath;
	cout<<"in path: "<< inpath << endl;

	QList<QPolygonF> outpathSort = Sort(QList<QPolygonF>(inpaths));
	sortpath = getTotalDistance(outpathSort);
	cout<<"Sort outpath: "<<  sortpath << endl;

	int bestpath=1;
	for (int i = 1; i < inpaths.size()/2; i++)
	{
		QList<QPolygonF> outpathTSP = GroupTSP(QList<QPolygonF>(inpaths),i);
		temppath = getTotalDistance(outpathTSP);
		//cout<<"xTSPSort outpaths: "<< temppath << " Groups " << i <<endl;
		if (temppath < tsppath)
		{
			bestpath = i;
			tsppath = temppath;
		}
		
	}
	QList<QPolygonF> outpathTSP = GroupTSP(QList<QPolygonF>(inpaths),bestpath);
	tsppath = getTotalDistance(outpathTSP);
	cout<<"TSPSort outpaths: "<< tsppath << " Groups " << bestpath <<endl;
	
	
	int bestpath2=1;
	for (int i = 1; i < outpathSort.size()/2; i++)
	{
		QList<QPolygonF> outpathTSP2 = GroupTSP(QList<QPolygonF>(outpathSort),i);
		temppath = getTotalDistance(outpathTSP2);
		//cout<<"xxTSPSort outpaths: "<< temppath << " Groups " << i <<endl;
		if (temppath < tsppath2)
		{
			bestpath2 = i;
			tsppath2 = temppath;
		}
		
	}
	QList<QPolygonF> outpathTSP2 = GroupTSP(QList<QPolygonF>(outpathSort),bestpath2);
	tsppath2 = getTotalDistance(outpathTSP2);
	cout<<"TSPSort outpath sort: "<< tsppath2 << " Groups " << bestpath2 <<endl;
	
	if(tsppath < tsppath2 && tsppath < tsppath2) return outpathTSP;
	if(tsppath2 < tsppath && tsppath2 < tsppath) return outpathTSP2;
	if(tsppath2 < sortpath && tsppath2 < inpath) return outpathTSP2;
	if(sortpath < inpath && sortpath < tsppath2) return outpathSort;
	if(inpath < sortpath && inpath < tsppath2 ) return inpaths;
	if(tsppath < sortpath && tsppath < inpath) return outpathTSP;
	if(sortpath < inpath && sortpath < tsppath) return outpathSort;
	if(inpath < sortpath && inpath < tsppath ) return inpaths;
	if(tsppath2 < tsppath) return outpathTSP2;
	if(tsppath < sortpath) return outpathTSP;
	if(sortpath < tsppath) return outpathSort;
	return inpaths;
}

QList<QPolygonF> PathSorter::BbSort (const QList<QPolygonF> inpaths)
{
	QList<QPolygonF> outpaths = QList<QPolygonF>(inpaths);
	for (int i = 0; i < (outpaths.size()-1); i++)
	{
		
		for (int j = (i+1); j < outpaths.size(); j++)
		{
		if (outpaths[i].boundingRect().intersects(outpaths[j].boundingRect()))
			{
				if (outpaths[i].boundingRect().width() > outpaths[j].boundingRect().width() || outpaths[i].boundingRect().height() > outpaths[j].boundingRect().height()) 
				{
					outpaths.swap(i,j);
				}
			}
		}
	}
	cout<<"BbSort outpaths: "<< getTotalDistance(outpaths) << endl;
	return outpaths;
}

QList<QPolygonF> PathSorter::GroupTSP(const QList<QPolygonF> inpaths, int groups)
{
	if (groups > inpaths.size()) groups = (int)((double) inpaths.size()-((double)inpaths.size()*0.2));
	if (groups < 1) groups = 1;
	QList<QList< QPolygonF> > listlistpath;
	QList<QPolygonF> temppaths;
	int inps = inpaths.size();
	int inpsparts = inps / groups;
	
	for (int i = 0; i < inps; i++)
	{
		if(i>=inpsparts)
		{
			listlistpath.append(temppaths);
			inpsparts += inps / groups;
			temppaths = QList<QPolygonF>() ;
		}
		temppaths.append(inpaths[i]);
	}
	listlistpath.append(temppaths);

	for (int i = 0; i < listlistpath.size(); i++)
	{
		listlistpath[i] = MyFakeTSP(listlistpath[i]);
	}
	
	QList<QPolygonF> outpaths;
	for (int i = 0; i < listlistpath.size(); i++)
	{
		for (int j = 0; j < listlistpath[i].size(); j++)
		{
			outpaths.append(listlistpath[i][j]);
		}
	}
	//cout<<"xTSPSort outpaths: "<< getTotalDistance(outpaths) << " Groups " << groups <<endl;
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
		if(i == 0) // find good start
		{
			qreal dist=10000.0;
			int bestindex=i;
			for (int j = (i+1); j < outpaths.size(); ++j)
			{
				if (getDistance(zero,outpaths[j]) < dist) 
				{
				dist = getDistance(zero,outpaths[j]);
				bestindex = j;
				}
				
			}
			if (dist != 0) outpaths.swap(0,bestindex);
		}
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
