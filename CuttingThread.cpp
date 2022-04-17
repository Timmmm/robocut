#include "CuttingThread.h"

#include "Plotter.h"

CuttingThread::CuttingThread(QObject* parent) : QThread(parent)
{
}

void CuttingThread::setParams(const CutParams& p)
{
	params = p;
}

void CuttingThread::run()
{
	SResult<> e = Cut(params);
	if (e.is_ok())
		emit success();
	else
		emit error(e.unwrap_err().c_str());
	exec();
}
