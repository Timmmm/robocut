#pragma once

#include <QDialog>

#include "CuttingThread.h"

namespace Ui {
	class CuttingDialog;
}

class CuttingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CuttingDialog(QWidget *parent = 0);
	~CuttingDialog();

	void startCut(QList<QPolygonF> cuts, int media, int speed, int pressure, bool trackenhancing);

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent* e);

private:
	Ui::CuttingDialog *ui;

	CuttingThread* thread;

private slots:
	void onSuccess();
	void onError(QString message);
};

