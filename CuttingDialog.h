#pragma once

#include <QDialog>

namespace Ui {
	class CuttingDialog;
}

class CuttingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CuttingDialog(QWidget *parent = 0);
	~CuttingDialog();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::CuttingDialog *ui;
};

