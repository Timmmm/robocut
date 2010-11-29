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

#include "CuttingThread.h"

#include "Plotter.h"

CuttingThread::CuttingThread(QObject *parent) :
    QThread(parent)
{
	_media = 300;
	_speed = 10;
	_pressure = 10;
	_trackenhancing = false;
	_regmark = false;
	_regsearch = false;
	_regwidth = 0.0;
	_reglength = 0.0;
}


void CuttingThread::setParams(QList<QPolygonF> cuts, double mediawidth, double mediaheight, int media,
                              int speed, int pressure, bool trackenhancing,
                              bool regmark, bool regsearch, float regwidth, float reglength)
{
	// TODO: Move all this into a structure.
	_cuts = cuts;
	_media = media;
	_speed = speed;
	_pressure = pressure;
	_trackenhancing = trackenhancing;
	_regmark = regmark;
	_regsearch = regsearch;
	_regwidth = regwidth;
	_reglength = reglength;
	_mediawidth = mediawidth;
	_mediaheight = mediaheight;
}


void CuttingThread::run()
{
	Error e = Cut(_cuts, _mediawidth, _mediaheight, _media, _speed, _pressure, _trackenhancing,
	              _regmark, _regsearch, _regwidth, _reglength);
	if (e)
		emit success();
	else
		emit error(e.message().c_str());
	exec();
}
