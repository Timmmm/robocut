#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	int err = libusb_init(NULL);
	if (err == LIBUSB_ERROR)
	{
		cerr << "Error initialising usb library." << endl;
		return 1;
	}

	// Block to ensure usb devices are closed.
	{
		QApplication a(argc, argv);
		MainWindow w;
		w.show();
		int ret = a.exec();
	}

	libusb_exit(NULL);
	return ret;
}
