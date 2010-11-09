#include <QtGui/QApplication>
#include "MainWindow.h"

#include <libusb-1.0/libusb.h>
#include <iostream>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

int main(int argc, char *argv[])
{
	int ret = 0;
	int err = libusb_init(NULL);
	if (err != LIBUSB_SUCCESS)
	{
		cerr << "Error initialising usb library." << endl;
		return 1;
	}
	// Code block to ensure usb devices are closed. Maybe.
	{
		QApplication a(argc, argv);
		MainWindow w;
		int index;
		int c;
		
		opterr = 0;
		while ((c = getopt (argc, argv, "stcf:")) != -1)
		{
			switch (c)
			{
			case 's':
				w.sortFlag = 1;
				break;
			case 't':
				w.tspFlag = 1;
				break;
			case 'c':
				w.cutFlag = 1;
				break;
			case 'f':
				w.fileValue = optarg;
				break;
			case '?':
			 if (optopt == 'f')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
				return 1;
			default:
				abort();
			}
		}
			printf ("sortFlag = %d, cutFlag = %d, fileValue = %s\n", w.sortFlag, w.cutFlag, w.fileValue);
		
			for (index = optind; index < argc; index++)
				printf ("Non-option argument %s\n", argv[index]);
		w.show();
		w.optDone();
		ret = a.exec();
	}

	libusb_exit(NULL);
	return ret;
}
