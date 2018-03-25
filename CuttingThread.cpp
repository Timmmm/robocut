#include "CuttingThread.h"

#include "Plotter.h"

CuttingThread::CuttingThread(QObject *parent) :
    QThread(parent)
{

}


void CuttingThread::setParams(const CutParams& p)
{
	params = p;
}


void CuttingThread::run()
{
	Error e = Cut(params);
	if (e)
		emit success();
	else
		emit error(e.message().c_str());
	exec();
}
