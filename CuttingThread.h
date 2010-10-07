#pragma once

#include <QThread>

#include <QPolygonF>

// A thread to send the cutting stuff to the plotter.
// To use:
//
// 1. Create.
// 2. Connect success() and error() to your class. Only one is called.
// 3. Call setParams(...) with the parameters you want.
// 4. Call start().
// 5. Wait for success() or error().

class CuttingThread : public QThread
{
	Q_OBJECT
public:
	explicit CuttingThread(QObject *parent = 0);

	void setParams(QList<QPolygonF> cuts, int media, int speed, int pressure, bool trackenhancing);

signals:
	void success();
	void error(QString message);
public slots:

protected:
	// This reads the params, tries to send the signals to the cutter and all that.
	// When it is done, it runs exec() to send the success() or error() signal.
	void run();

private:
	QList<QPolygonF> _cuts;
	int _media;
	int _speed;
	int _pressure;
	bool _trackenhancing;
};
