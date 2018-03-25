#pragma once

#include <QDialog>

#include "CuttingThread.h"

namespace Ui {
	class CuttingDialog;
}

// This is the dialog that shows while the cut is progressing. The thread that does the cutting
// is also owned by the dialog, in a slightly bad design.
class CuttingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CuttingDialog(QWidget *parent = nullptr);
	~CuttingDialog();

	// Start the cutting thread. Call this only once, before the dialog is shown.
	// It creates the thread, passes it the cutting details, and runs it.
	void startCut(const CutParams& params);

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent* e);

private:
	Ui::CuttingDialog *ui;

	// The cutting thread. It is pretty basic - just one long run() routine and then it calls onSuccess() or
	// onError().
	CuttingThread* thread;

private slots:
	// These are called from the thread when it has finished. Only one is called.
	void onSuccess();
	void onError(QString message);
};

