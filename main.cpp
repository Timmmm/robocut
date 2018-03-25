#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"

#include "ProgramOptions.h"

#include <libusb/libusb.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	// ROBOCUT_VERSION is defined in the build system.
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
