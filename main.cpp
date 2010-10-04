#include <QtGui/QApplication>
#include "MainWindow.h"

#include <libusb-1.0/libusb.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	int err = libusb_init(NULL);
	if (err != LIBUSB_SUCCESS)
	{
		cerr << "Error initialising usb library." << endl;
		return 1;
	}

	int ret = 0;

	// Code block to ensure usb devices are closed.
	{
		QApplication a(argc, argv);
		MainWindow w;
		w.show();
		ret = a.exec();
	}

	libusb_exit(NULL);
	return ret;
}
