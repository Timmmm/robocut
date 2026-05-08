#include "CutDialog.h"
#include "ui_CutDialog.h"

CutDialog::CutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::CutDialog)
{
	ui->setupUi(this);
	//	ui->mediaCombo->setCurrentIndex(ProgramOptions::Instance().getMedia());
	//	ui->speedSlider->setValue(ProgramOptions::Instance().getSpeed());
	//	ui->pressureSlider->setValue(ProgramOptions::Instance().getPressure());
	//	ui->trackEnhancingCheckbox->setChecked(ProgramOptions::Instance().getTrackEnhancing());
	//	if(ProgramOptions::Instance().getRegMark())
	//	{
	//		ui->regMarksGroup->setChecked(true);
	//		ui->regSearchCheckbox->setChecked(false);
	//	}
	//	if(ProgramOptions::Instance().getRegMarkAuto())
	//	{
	//		ui->regMarksGroup->setChecked(true);
	//		ui->regSearchCheckbox->setChecked(true);
	//	}
	//	ui->regWidthSpinner->setValue(ProgramOptions::Instance().getRegDimensionWidthMM());
	//	ui->regHeightSpinner->setValue(ProgramOptions::Instance().getRegDimensionHeightMM());
}

CutDialog::~CutDialog()
{
	delete ui;
}

void CutDialog::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

int CutDialog::media() const
{
	int idx = ui->mediaCombo->currentIndex();

	// Media code for each entry.
	int medias[] = {100, 101, 102, 106, 111, 112, 113, 120, 121, 122, 123, 124, 125,
	                126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138};

	if (idx < 0 || idx >= 26)
		idx = 0;
	return medias[idx];
}

int CutDialog::speed() const
{
	return ui->speedSlider->value();
}

int CutDialog::pressure() const
{
	return ui->pressureSlider->value();
}

bool CutDialog::trackEnhancing() const
{
	return ui->trackEnhancingCheckbox->isChecked();
}

bool CutDialog::regMark() const
{
	return ui->regMarksGroup->isChecked();
}

bool CutDialog::regSearch() const
{
	return ui->regSearchCheckbox->isChecked();
}

double CutDialog::regWidth() const
{
	return ui->regWidthSpinner->value();
}

double CutDialog::regHeight() const
{
	return ui->regHeightSpinner->value();
}

void CutDialog::onMediaChanged(int idx)
{
	if (idx < 0 || idx >= 26)
		idx = 0;

	// Pressure for each entry.
	int pressures[] = {27, 27, 10, 14, 27, 2,  10, 30, 30, 30, 30, 1,  1,
	                   1,  30, 20, 27, 30, 30, 5,  25, 20, 20, 30, 30, 30};

	// Default speed is max except for magnetic sheet.
	int newspeed = idx == 17 ? 3 : 10;
	int newpressure = pressures[idx];

	ui->speedSlider->setValue(newspeed);
	ui->pressureSlider->setValue(newpressure);
}
