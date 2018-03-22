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

#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"

#include "ProgramOptions.h"

#include <libusb/libusb.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	// Version is defined in Robocut.pro
	ProgramOptions::Instance().setVersion(ROBOCUT_VERSION);

	// Initialise options from command line if specified.
	ProgramOptions::Instance().GetOpt(argc, argv);

	int err = libusb_init(nullptr);
	if (err != LIBUSB_SUCCESS)
	{
		QMessageBox::critical(nullptr, "Error initialising USB library.", libusb_error_name(err));
		cerr << "Error initialising USB library: " << libusb_error_name(err) << endl;
		return 1;
	}

	int ret = 0;

	// Code block to ensure usb devices are closed. Maybe.
	{
		QApplication a(argc, argv);
		MainWindow w; 
		ret = a.exec();
	}

	libusb_exit(nullptr);
	return ret;
}
