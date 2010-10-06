#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>

namespace Ui {
    class CutDialog;
}

class CutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();

	int media() const;
	int speed() const;
	int pressure() const;
	bool trackEnhancing() const;

protected:
	void changeEvent(QEvent *e);

private slots:
	void onMediaChanged(int idx);
private:
    Ui::CutDialog *ui;
};

#endif // CUTDIALOG_H
