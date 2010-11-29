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

#include <QThread>

#include <QPolygonF>

// A thread to send the cutting stuff to the plotter.
// To use:
//
// 1. Create.
// 2. Connect success() and error() to your class. Only one is called.
// 3. Call setParams(...) with the parameters you want.
// 4. Call start().
// 5. Wait for success() or error().

class CuttingThread : public QThread
{
	Q_OBJECT
public:
	explicit CuttingThread(QObject *parent = 0);

	// Set the parameters to use for the cut.
	void setParams(QList<QPolygonF> cuts, double mediawidth, double mediaheight, int media, int speed, int pressure,
	               bool trackenhancing, bool regmark, bool regsearch, float regwidth, float reglength);

signals:
	// Emitted if the cutting was (as far as we can tell) successful.
	void success();
	// Emitted if the cutting failed.
	void error(QString message);
public slots:

protected:
	// This reads the params, then tries to send the data to the cutter.
	// When it is done, it runs exec() to send the success() or error() signal.
	void run();

private:
	QList<QPolygonF> _cuts;
	double _mediawidth;
	double _mediaheight;
	int _media;
	int _speed;
	int _pressure;
	bool _trackenhancing;
	bool _regmark; 
	bool _regsearch; 
	double _regwidth;
	double _reglength;
};
