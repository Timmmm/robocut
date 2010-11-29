/***************************************************************************
 *   This file is part of Robocut.                                         *
 *   Copyright (C) 2010 Tim Hutt <tdhutt@gmail.com>                        *
 *   Copyright (C) 2010 Markus Schulz <schulz@alpharesearch.de>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#include "CuttingDialog.h"
#include "ui_CuttingDialog.h"

#include <QMessageBox>
#include <QPushButton>
#include <QCloseEvent>

#include <QDebug>

CuttingDialog::CuttingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CuttingDialog)
{
	ui->setupUi(this);
	thread = NULL;
}

CuttingDialog::~CuttingDialog()
{
	delete ui;
	if (thread)
	{
		thread->exit();
		thread->wait(1000);
	}
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


void CuttingDialog::startCut(QList<QPolygonF> cuts, double mediawidth, double mediaheight, int media, int speed,
                             int pressure, bool trackenhancing,
							 bool regmark, bool regsearch, double regwidth, double reglength)
{
	if (thread)
	{
		qDebug() << "Internal error: startCut() called twice.";
		return;
	}
	thread = new CuttingThread(this);

	connect(thread, SIGNAL(success()), SLOT(onSuccess()));
	connect(thread, SIGNAL(error(QString)), SLOT(onError(QString)));

	thread->setParams(cuts, mediawidth, mediaheight, media, speed, pressure, trackenhancing,
	                  regmark, regsearch, regwidth, reglength);

	thread->start();
}

void CuttingDialog::onSuccess()
{
	ui->status->setText("Finished");
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Close");
	thread->exit();
	thread->wait(1000);
	delete thread;
	thread = NULL;
}

void CuttingDialog::onError(QString message)
{
	ui->status->setText("Error: " + message);
	ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Close");
	thread->exit();
	thread->wait(1000);
	delete thread;
	thread = NULL;
}

void CuttingDialog::closeEvent(QCloseEvent* e)
{
	if (thread)
	{
		if (QMessageBox::question(this, "Kill cutting process?", "Are you sure you wish to stop the cutting process? "
								  "This may result in bad stuff. It might be better just to turn the plotter off and on again.",
								  QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
		{
			e->ignore();
			return;
		}

		thread->terminate();
		thread->wait(5000);
		delete thread;
		thread = NULL;
	}
	return QDialog::closeEvent(e);
}
