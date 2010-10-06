#include "CuttingDialog.h"
#include "ui_CuttingDialog.h"

CuttingDialog::CuttingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CuttingDialog)
{
    ui->setupUi(this);
}

CuttingDialog::~CuttingDialog()
{
    delete ui;
}

void CuttingDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
