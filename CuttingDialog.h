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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

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
	explicit CuttingDialog(QWidget *parent = 0);
	~CuttingDialog();

	// Start the cutting thread. Call this only once, before the dialog is shown.
	// It creates the thread, passes it the cutting details, and runs it.
	void startCut(QList<QPolygonF> cuts, double mediawidth, double mediaheigt, int media, int speed,
	              int pressure, bool trackenhancing, bool regmark, bool regsearch,
	              double regwidth, double reglength);

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

