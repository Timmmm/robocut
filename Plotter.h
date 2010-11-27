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

#include <QList>
#include <QPolygonF>
#include "Common.h"

using namespace std;

// Perform the cut. Returns error state. Note that this just sends the data, it has no way of knowing whether it really
// worked and there is no easy way to cancel it.
//
// media: The type of media. I'm not sure what this effects, see CuttingDialog.cpp/ui for values. 300 is "custom".
//        The value is not fully checked for sanity.
// speed: Cutting speed: 1-10 inclusive.
// pressure: Cutting pressure: 1-33 inclusive.
// trackenhancing: Supposed to feed/unfeed the media a few times to create tracks.
// regmark: Whether to use registration marks for calibration.
// regsearch: ?
// regwidth/height: Distance between the registration marks.
//
// TODO: Apparently you can change the number of registration marks?
Error Cut(QList<QPolygonF> cuts, double mediawidth, double mediaheight, int media, int speed, int pressure,
		  bool trackenhancing, bool regmark, bool regsearch,
		  double regwidth, double reglength);
