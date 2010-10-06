#ifndef CUTTINGTHREAD_H
#define CUTTINGTHREAD_H

#include <QThread>

class CuttingThread : public QThread
{
    Q_OBJECT
public:
    explicit CuttingThread(QObject *parent = 0);

signals:

public slots:

};

#endif // CUTTINGTHREAD_H
