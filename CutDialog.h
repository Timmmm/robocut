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
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();

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

	// Whether to search.
	bool regMark() const;
	bool regSearch() const; 
	// Positions of the registration marks.
	double regWidth() const;
	double regHeight() const;

protected:
	void changeEvent(QEvent *e);

private slots:
	// When they change the media selection, update the default speed and pressure.
	void onMediaChanged(int idx);
private:
    Ui::CutDialog *ui;
};
