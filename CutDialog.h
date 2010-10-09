#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>

namespace Ui {
    class CutDialog;
}

// This dialog asks the user what settings to use for cutting.
class CutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();

	// The media code. See CutDialog.ui for a list.
	int media() const;
	// The speed (1-10).
	int speed() const;
	// The cutting pressure (1-33).
	int pressure() const;
	// Whatever track enhancing is...
	bool trackEnhancing() const;

protected:
	void changeEvent(QEvent *e);

private slots:
	// When they change the media, update the default speed and pressure.
	void onMediaChanged(int idx);
private:
    Ui::CutDialog *ui;
};

#endif // CUTDIALOG_H
