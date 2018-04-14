#include <iostream>

#include <QApplication>
#include <QMessageBox>
#include <QCommandLineParser>

#include "MainWindow.h"
#include "CommandLineOptions.h"

#include <libusb/libusb.h>

using namespace std;

int main(int argc, char *argv[])
{
	// For the benefit of QSettings.
	QCoreApplication::setOrganizationName("Robocut");
    QCoreApplication::setOrganizationDomain("robocut.com");
    QCoreApplication::setApplicationName("Robocut");
	
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
		
//		processCommandLineOptions(a, argc, argv);
		
		MainWindow w; 
		ret = a.exec();
	}

	libusb_exit(nullptr);
	return ret;
}
