#pragma once

#include <QDialog>

namespace Ui {
    class CutDialog;
}

// This dialog asks the user what settings to use for cutting.
class CutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CutDialog(QWidget *parent = nullptr);
	~CutDialog() override;

	// The media code. See CutDialog.ui for a list.
	int media() const;
	// The speed (1-10).
	int speed() const;
	// The cutting pressure (1-33).
	int pressure() const;
	// Track enhancing. This is when the cutter rolls the media backwards and forwards
	// a few times before cutting in order to indent it with tracks where the rollors are.
	// The idea is that it will slip less after that is done.
	bool trackEnhancing() const;

	// Whether registration marks are enabled.
	bool regMark() const;
	// If true, search for registration marks automatically, otherwise
	// rely on manual positioning?
	bool regSearch() const; 
	// Positions of the registration marks.
	double regWidth() const;
	double regHeight() const;

protected:
	void changeEvent(QEvent *e) override;

private slots:
	// When they change the media selection, update the default speed and pressure.
	void onMediaChanged(int idx);
private:
    Ui::CutDialog *ui;
};
